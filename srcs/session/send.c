#include "session.h"
#include "../debug/debug.h"

typedef struct s_send_info {
	int				ttl;
	int				probe_index;
	unsigned short	port;
	struct timeval	sent_at;
	struct timeval	deadline;
	ssize_t			sent_len;
}	t_send_info;

static int	time_is_reached(const struct timeval *now,
		const struct timeval *target) {
	if (now->tv_sec > target->tv_sec)
		return (1);
	if (now->tv_sec == target->tv_sec && now->tv_usec >= target->tv_usec)
		return (1);
	return (0);
}

static void	add_seconds_to_timeval(struct timeval *dst,
		const struct timeval *src, double seconds) {
	long	sec;
	long	usec;

	sec = (long)seconds;
	usec = (long)((seconds - sec) * 1000000.0);
	dst->tv_sec = src->tv_sec + sec;
	dst->tv_usec = src->tv_usec + usec;
	if (dst->tv_usec >= 1000000)
	{
		dst->tv_sec += dst->tv_usec / 1000000;
		dst->tv_usec %= 1000000;
	}
}

static int	send_is_allowed_now(const t_traceroute_config *config,
		const t_session *session, const struct timeval *now) {
	if (session->scheduler.destination_found
		&& session->scheduler.send_ttl > session->scheduler.destination_ttl)
		return (0);
	if (session->scheduler.send_ttl > config->cli.max_hops)
		return (0);
	if (session->scheduler.in_flight_count >= session->probe_capacity)
		return (0);
	if (!time_is_reached(now, &session->scheduler.next_send_at))
		return (0);
	return (1);
}

static t_probe	*find_free_probe(t_session *session) {
	int	i;

	i = 0;
	while (i < session->probe_capacity)
	{
		if (session->probes[i].state == PROBE_FREE)
			return (&session->probes[i]);
		i++;
	}
	return (NULL);
}

static void	build_next_probe_info(const t_traceroute_config *config,
		t_session *session, t_send_info *info) {
	info->ttl = session->scheduler.send_ttl;
	info->probe_index = session->scheduler.send_probe_index;
	info->port = session->scheduler.next_port;
	gettimeofday(&info->sent_at, NULL);
	add_seconds_to_timeval(&info->deadline, &info->sent_at,
		config->cli.wait_time);
}

static void	build_udp_destination(const t_traceroute_config *config,
		const t_send_info *info, struct sockaddr_in *dest) {
	*dest = config->target.addr;
	dest->sin_port = htons(info->port);
}

static void	commit_sent_probe(t_probe *probe, const t_send_info *info) {
	probe->state = PROBE_SENT;
	probe->ttl = info->ttl;
	probe->probe_index = info->probe_index;
	probe->port = info->port;
	probe->sent_at = info->sent_at;
	probe->deadline = info->deadline;
	probe->reply_addr.sin_family = 0;
	probe->reply_addr.sin_port = 0;
	probe->reply_addr.sin_addr.s_addr = 0;
	probe->rtt_us = 0;
	probe->icmp_type = 0;
	probe->icmp_code = 0;
}

static void	advance_send_scheduler(const t_traceroute_config *config,
		t_session *session, const t_send_info *info) {
	session->scheduler.in_flight_count++;
	session->scheduler.next_port++;
	session->scheduler.next_send_at = info->sent_at;
	add_seconds_to_timeval(&session->scheduler.next_send_at,
		&info->sent_at, config->cli.send_wait);
	session->scheduler.send_probe_index++;
	if (session->scheduler.send_probe_index >= config->cli.probes_per_hop)
	{
		session->scheduler.send_probe_index = 0;
		session->scheduler.send_ttl++;
	}
}

static int	send_one_probe(t_traceroute_config *config, t_session *session,
		t_probe *probe, int *exit_status) {
	t_send_info			info;
	struct sockaddr_in	dest;

	build_next_probe_info(config, session, &info);
	build_udp_destination(config, &info, &dest);
	if (traceroute_set_socket_ttl(config, info.ttl) < 0)
	{
		traceroute_print_error(config);
		*exit_status = 1;
		return (0);
	}
	info.sent_len = sendto(config->socket.udp_fd, config->packet.payload,
		config->packet.payload_len, 0, (struct sockaddr *)&dest, sizeof(dest));
	if (info.sent_len < 0)
	{
		config->socket.error = errno;
		traceroute_print_error(config);
		*exit_status = 1;
		return (0);
	}
	commit_sent_probe(probe, &info);
	advance_send_scheduler(config, session, &info);
	DEBUG_SEND(config, probe, info.sent_len);
	return (1);
}

int	traceroute_session_send_ready(t_traceroute_config *config,
		t_session *session, int *exit_status) {
	struct timeval	now;
	t_probe			*probe;

	while (1)
	{
		gettimeofday(&now, NULL);
		if (!send_is_allowed_now(config, session, &now))
			return (1);
		probe = find_free_probe(session);
		if (!probe)
			return (1);
		if (!send_one_probe(config, session, probe, exit_status))
			return (0);
	}
}
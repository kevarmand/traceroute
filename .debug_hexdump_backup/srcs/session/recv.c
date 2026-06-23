

#include "session.h"
#include "packet.h"
#include "../debug/debug.h"

#define TRACEROUTE_MIN_ADAPTIVE_TIMEOUT_US 3000L

static int	reply_is_available(t_traceroute_config *config,
		int *exit_status) {
	fd_set			read_fds;
	struct timeval	timeout;
	int				ready;

	FD_ZERO(&read_fds);
	FD_SET(config->socket.icmp_fd, &read_fds);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	ready = select(config->socket.icmp_fd + 1, &read_fds, NULL, NULL,
		&timeout);
	if (ready < 0)
	{
		config->socket.error = errno;
		traceroute_print_error(config);
		*exit_status = 1;
		return (-1);
	}
	return (ready > 0 && FD_ISSET(config->socket.icmp_fd, &read_fds));
}

static int	time_is_before(const struct timeval *left,
		const struct timeval *right) {
	if (left->tv_sec < right->tv_sec)
		return (1);
	if (left->tv_sec == right->tv_sec && left->tv_usec < right->tv_usec)
		return (1);
	return (0);
}

static void	add_usec_to_timeval(struct timeval *dst,
		const struct timeval *src, long usec) {
	dst->tv_sec = src->tv_sec + usec / 1000000;
	dst->tv_usec = src->tv_usec + usec % 1000000;
	if (dst->tv_usec >= 1000000)
	{
		dst->tv_sec++;
		dst->tv_usec -= 1000000;
	}
}

static long	max_wait_us(const t_traceroute_config *config) {
	return ((long)(config->cli.wait_time * 1000000.0));
}

static long	adaptive_wait_us(const t_traceroute_config *config,
		double factor, long rtt_us) {
	long	wait_us;
	long	max_us;

	if (factor <= 0.0)
		return (0);
	wait_us = (long)(factor * (double)rtt_us);
	if (wait_us < TRACEROUTE_MIN_ADAPTIVE_TIMEOUT_US)
		wait_us = TRACEROUTE_MIN_ADAPTIVE_TIMEOUT_US;
	max_us = max_wait_us(config);
	if (wait_us > max_us)
		wait_us = max_us;
	return (wait_us);
}

static void	shorten_probe_deadline(t_probe *probe, long wait_us) {
	struct timeval	deadline;

	if (wait_us <= 0)
		return;
	add_usec_to_timeval(&deadline, &probe->sent_at, wait_us);
	if (time_is_before(&deadline, &probe->deadline))
		probe->deadline = deadline;
}

static void	shorten_same_hop_deadlines(const t_traceroute_config *config,
		t_session *session, const t_probe *reply_probe) {
	long	wait_us;
	int		i;

	wait_us = adaptive_wait_us(config, config->cli.wait_here,
		reply_probe->rtt_us);
	i = 0;
	while (i < session->probe_capacity)
	{
		if (session->probes[i].state == PROBE_SENT
			&& session->probes[i].ttl == reply_probe->ttl)
			shorten_probe_deadline(&session->probes[i], wait_us);
		i++;
	}
}

static void	shorten_near_hop_deadlines(const t_traceroute_config *config,
		t_session *session, const t_probe *reply_probe) {
	long	wait_us;
	int		i;

	wait_us = adaptive_wait_us(config, config->cli.wait_near,
		reply_probe->rtt_us);
	i = 0;
	while (i < session->probe_capacity)
	{
		if (session->probes[i].state == PROBE_SENT
			&& session->probes[i].ttl < reply_probe->ttl)
			shorten_probe_deadline(&session->probes[i], wait_us);
		i++;
	}
}

static void	update_adaptive_deadlines(const t_traceroute_config *config,
		t_session *session, const t_probe *reply_probe) {
	shorten_same_hop_deadlines(config, session, reply_probe);
	shorten_near_hop_deadlines(config, session, reply_probe);
}

static int	reply_matches_target(const t_traceroute_config *config,
		const t_icmp_reply *reply) {
	if (reply->original_dst.s_addr != config->target.addr.sin_addr.s_addr)
		return (0);
	return (1);
}

static t_probe	*find_sent_probe_by_port(t_session *session,
		unsigned short port) {
	int			i;
	t_probe		*probe;

	i = 0;
	while (i < session->probe_capacity)
	{
		probe = &session->probes[i];
		if (probe->state == PROBE_SENT && probe->port == port)
			return (probe);
		i++;
	}
	return (NULL);
}

static long	time_diff_us(const struct timeval *start,
		const struct timeval *end) {
	long	sec;
	long	usec;

	sec = end->tv_sec - start->tv_sec;
	usec = end->tv_usec - start->tv_usec;
	return (sec * 1000000 + usec);
}

static int	is_terminal_reply(const t_icmp_reply *reply) {
	if (reply->icmp_type == ICMP_DEST_UNREACH)
		return (1);
	return (0);
}

static void	mark_stop_ttl(t_session *session, int ttl) {
	if (!session->scheduler.stop_found
		|| ttl < session->scheduler.stop_ttl)
		session->scheduler.stop_ttl = ttl;
	session->scheduler.stop_found = 1;
}

static void	commit_reply(t_traceroute_config *config, t_session *session,
		t_probe *probe, const t_icmp_reply *reply) {
	struct timeval	now;

	gettimeofday(&now, NULL);
	probe->state = PROBE_REPLY_PENDING;
	probe->reply_addr = reply->from;
	probe->rtt_us = time_diff_us(&probe->sent_at, &now);
	probe->icmp_type = reply->icmp_type;
	probe->icmp_code = reply->icmp_code;
	session->scheduler.in_flight_count--;
	update_adaptive_deadlines(config, session, probe);
	if (is_terminal_reply(reply))
		mark_stop_ttl(session, probe->ttl);
}

static int	read_one_reply(t_traceroute_config *config,
		t_session *session, int *exit_status) {
	unsigned char		buffer[1500];
	struct sockaddr_in	from;
	socklen_t			from_len;
	ssize_t				length;
	t_icmp_reply		reply;
	t_icmp_parse_status	status;
	t_probe				*probe;

	from_len = sizeof(from);
	length = recvfrom(config->socket.icmp_fd, buffer, sizeof(buffer), 0,
		(struct sockaddr *)&from, &from_len);
	if (length < 0)
	{
		config->socket.error = errno;
		traceroute_print_error(config);
		*exit_status = 1;
		return (0);
	}
	status = traceroute_parse_icmp_reply(buffer, length, &from, &reply);
	if (status != ICMP_PARSE_OK)
	{
		DEBUG_RECV_IGNORED(config, &from, length, status);
		return (1);
	}
	if (!reply_matches_target(config, &reply))
		return (1);
	probe = find_sent_probe_by_port(session, reply.port);
	DEBUG_RECV(config, &reply, probe);
	if (!probe)
		return (1);
	commit_reply(config, session, probe, &reply);
	return (1);
}

int	traceroute_session_drain_replies(t_traceroute_config *config,
		t_session *session, int *exit_status) {
	int	available;

	while (1)
	{
		available = reply_is_available(config, exit_status);
		if (available < 0)
			return (0);
		if (!available)
			return (1);
		if (!read_one_reply(config, session, exit_status))
			return (0);
	}
}



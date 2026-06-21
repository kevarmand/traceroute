#include "session.h"
#include "packet.h"
#include "../debug/debug.h"

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

static int	is_final_destination_reply(const t_traceroute_config *config,
		const t_icmp_reply *reply) {
	if (reply->from.sin_addr.s_addr != config->target.addr.sin_addr.s_addr)
		return (0);
	if (reply->icmp_type == ICMP_DEST_UNREACH
		&& reply->icmp_code == ICMP_PORT_UNREACH)
		return (1);
	return (0);
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
	if (is_final_destination_reply(config, reply))
	{
		session->scheduler.destination_found = 1;
		session->scheduler.destination_ttl = probe->ttl;
	}
}

static int	read_one_reply(t_traceroute_config *config,
		t_session *session, int *exit_status) {
	unsigned char		buffer[1500];
	struct sockaddr_in	from;
	socklen_t			from_len;
	ssize_t				length;
	t_icmp_reply		reply;
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
	if (!traceroute_parse_icmp_reply(buffer, length, &from, &reply))
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

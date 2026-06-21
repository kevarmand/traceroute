#include "session.h"

static int	time_is_reached(const struct timeval *now,
		const struct timeval *target) {
	if (now->tv_sec > target->tv_sec)
		return (1);
	if (now->tv_sec == target->tv_sec && now->tv_usec >= target->tv_usec)
		return (1);
	return (0);
}

static t_probe	*find_printable_probe(t_session *session) {
	int			i;
	t_probe		*probe;

	i = 0;
	while (i < session->probe_capacity)
	{
		probe = &session->probes[i];
		if (probe->ttl == session->scheduler.print_ttl
			&& probe->probe_index == session->scheduler.print_probe_index
			&& (probe->state == PROBE_REPLY_PENDING
				|| probe->state == PROBE_TIMEOUT_PENDING))
			return (probe);
		i++;
	}
	return (NULL);
}

static void	start_hop_line(t_session *session) {
	if (!session->output.line_started)
	{
		printf("%2d ", session->scheduler.print_ttl);
		session->output.line_started = 1;
	}
}

static void	finish_hop_line(const t_traceroute_config *config,
		t_session *session) {
	if (session->scheduler.print_probe_index < config->cli.probes_per_hop)
		return;
	printf("\n");
	session->scheduler.print_probe_index = 0;
	session->scheduler.print_ttl++;
	session->output.line_started = 0;
	session->output.has_last_addr = 0;
}

static void	print_timeout_probe(t_probe *probe) {
	printf(" *");
	probe->state = PROBE_FREE;
}

static int	is_same_addr(const struct sockaddr_in *left,
		const struct sockaddr_in *right) {
	return (left->sin_addr.s_addr == right->sin_addr.s_addr);
}

static void	print_reply_addr(t_session *session, const t_probe *probe) {
	if (!session->output.has_last_addr
		|| !is_same_addr(&session->output.last_addr, &probe->reply_addr))
	{
		printf(" %s", inet_ntoa(probe->reply_addr.sin_addr));
		session->output.last_addr = probe->reply_addr;
		session->output.has_last_addr = 1;
	}
}

static void	print_reply_probe(t_session *session, t_probe *probe) {
	print_reply_addr(session, probe);
	printf("  %.3f ms", probe->rtt_us / 1000.0);
	probe->state = PROBE_FREE;
}

void	traceroute_session_expire_probes(t_traceroute_config *config,
		t_session *session) {
	struct timeval	now;
	int				i;

	(void)config;
	gettimeofday(&now, NULL);
	i = 0;
	while (i < session->probe_capacity)
	{
		if (session->probes[i].state == PROBE_SENT
			&& time_is_reached(&now, &session->probes[i].deadline))
		{
			session->probes[i].state = PROBE_TIMEOUT_PENDING;
			session->scheduler.in_flight_count--;
		}
		i++;
	}
}

void	traceroute_session_print_ready(t_traceroute_config *config,
		t_session *session) {
	t_probe	*probe;

	while (1)
	{
		probe = find_printable_probe(session);
		if (!probe)
			return;
		start_hop_line(session);
		if (probe->state == PROBE_TIMEOUT_PENDING)
			print_timeout_probe(probe);
		else if (probe->state == PROBE_REPLY_PENDING)
			print_reply_probe(session, probe);
		session->scheduler.print_probe_index++;
		finish_hop_line(config, session);
	}
}

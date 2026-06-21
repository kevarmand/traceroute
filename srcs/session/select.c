#include "session.h"

static int	time_is_before(const struct timeval *left,
		const struct timeval *right) {
	if (left->tv_sec < right->tv_sec)
		return (1);
	if (left->tv_sec == right->tv_sec && left->tv_usec < right->tv_usec)
		return (1);
	return (0);
}

static int	time_is_reached(const struct timeval *now,
		const struct timeval *target) {
	if (now->tv_sec > target->tv_sec)
		return (1);
	if (now->tv_sec == target->tv_sec && now->tv_usec >= target->tv_usec)
		return (1);
	return (0);
}

static void	time_subtract(struct timeval *dst, const struct timeval *target,
		const struct timeval *now) {
	dst->tv_sec = target->tv_sec - now->tv_sec;
	dst->tv_usec = target->tv_usec - now->tv_usec;
	if (dst->tv_usec < 0)
	{
		dst->tv_sec--;
		dst->tv_usec += 1000000;
	}
	if (dst->tv_sec < 0)
	{
		dst->tv_sec = 0;
		dst->tv_usec = 0;
	}
}

static int	has_more_to_send(const t_traceroute_config *config,
		const t_session *session) {
	if (session->scheduler.destination_found
		&& session->scheduler.send_ttl > session->scheduler.destination_ttl)
		return (0);
	if (session->scheduler.send_ttl > config->cli.max_hops)
		return (0);
	return (1);
}

static int	has_free_probe(const t_session *session) {
	int	i;

	i = 0;
	while (i < session->probe_capacity)
	{
		if (session->probes[i].state == PROBE_FREE)
			return (1);
		i++;
	}
	return (0);
}

static int	find_earliest_deadline(const t_session *session,
		struct timeval *deadline) {
	int	found;
	int	i;

	found = 0;
	i = 0;
	while (i < session->probe_capacity)
	{
		if (session->probes[i].state == PROBE_SENT
			&& (!found || time_is_before(&session->probes[i].deadline,
					deadline)))
		{
			*deadline = session->probes[i].deadline;
			found = 1;
		}
		i++;
	}
	return (found);
}

static void	keep_earliest_event(int *found, struct timeval *event,
		const struct timeval *candidate) {
	if (!*found || time_is_before(candidate, event))
	{
		*event = *candidate;
		*found = 1;
	}
}

static int	find_next_event(const t_traceroute_config *config,
		const t_session *session, struct timeval *event) {
	struct timeval	deadline;
	int				found;

	found = 0;
	if (has_more_to_send(config, session)
		&& session->scheduler.in_flight_count < session->probe_capacity
		&& has_free_probe(session))
		keep_earliest_event(&found, event, &session->scheduler.next_send_at);
	if (find_earliest_deadline(session, &deadline))
		keep_earliest_event(&found, event, &deadline);
	return (found);
}

static int	wait_for_icmp(t_traceroute_config *config,
		struct timeval *timeout, int *exit_status) {
	fd_set	read_fds;
	int		ready;

	FD_ZERO(&read_fds);
	FD_SET(config->socket.icmp_fd, &read_fds);
	ready = select(config->socket.icmp_fd + 1, &read_fds, NULL, NULL, timeout);
	if (ready < 0)
	{
		config->socket.error = errno;
		traceroute_print_error(config);
		*exit_status = 1;
		return (0);
	}
	return (1);
}

int	traceroute_session_wait(t_traceroute_config *config,
		t_session *session, int *exit_status) {
	struct timeval	now;
	struct timeval	event;
	struct timeval	timeout;

	if (traceroute_session_is_finished(config, session))
		return (1);
	if (!find_next_event(config, session, &event))
		return (1);
	gettimeofday(&now, NULL);
	if (time_is_reached(&now, &event))
		return (1);
	time_subtract(&timeout, &event, &now);
	return (wait_for_icmp(config, &timeout, exit_status));
}
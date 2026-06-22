
#include "session.h"

static int	time_is_reached(const struct timeval *now,
		const struct timeval *target) {
	if (now->tv_sec > target->tv_sec)
		return (1);
	if (now->tv_sec == target->tv_sec && now->tv_usec >= target->tv_usec)
		return (1);
	return (0);
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


#include "session.h"

int	traceroute_session_is_finished(const t_traceroute_config *config,
		const t_session *session) {
	if (session->scheduler.stop_found
		&& session->scheduler.print_ttl > session->scheduler.stop_ttl)
		return (1);
	if (!session->scheduler.stop_found
		&& session->scheduler.print_ttl > config->cli.max_hops)
		return (1);
	return (0);
}


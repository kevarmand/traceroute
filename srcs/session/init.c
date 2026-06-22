
#include "session.h"

static void	init_session_empty(t_session *session) {
	session->probes = NULL;
	session->probe_capacity = 0;
	session->total_probes = 0;
}

static int	compute_total_probes(const t_traceroute_config *config) {
	return ((config->cli.max_hops - config->cli.first_ttl + 1)
		* config->cli.probes_per_hop);
}

static int	compute_probe_capacity(const t_traceroute_config *config,
		int total_probes) {
	int	capacity;

	capacity = total_probes;
	if (capacity > config->cli.sim_queries)
		capacity = config->cli.sim_queries;
	if (capacity > TRACEROUTE_MAX_PROBE_SLOTS)
		capacity = TRACEROUTE_MAX_PROBE_SLOTS;
	return (capacity);
}

static void	init_payload(t_traceroute_config *config) {
	size_t	i;

	i = 0;
	while (i < config->packet.payload_len)
	{
		config->packet.payload[i] = '@' + i;
		i++;
	}
}

static void	init_probe(t_probe *probe) {
	probe->state = PROBE_FREE;
	probe->ttl = 0;
	probe->probe_index = 0;
	probe->port = 0;
	probe->sent_at.tv_sec = 0;
	probe->sent_at.tv_usec = 0;
	probe->deadline.tv_sec = 0;
	probe->deadline.tv_usec = 0;
	probe->reply_addr.sin_family = 0;
	probe->reply_addr.sin_port = 0;
	probe->reply_addr.sin_addr.s_addr = 0;
	probe->rtt_us = 0;
	probe->icmp_type = 0;
	probe->icmp_code = 0;
}

static void	init_probes(t_session *session) {
	int	i;

	i = 0;
	while (i < session->probe_capacity)
	{
		init_probe(&session->probes[i]);
		i++;
	}
}

static void	init_scheduler(const t_traceroute_config *config,
		t_session *session) {
	session->scheduler.send_ttl = config->cli.first_ttl;
	session->scheduler.send_probe_index = 0;
	session->scheduler.print_ttl = config->cli.first_ttl;
	session->scheduler.print_probe_index = 0;
	session->scheduler.next_port = config->cli.base_port;
	session->scheduler.next_send_at.tv_sec = 0;
	session->scheduler.next_send_at.tv_usec = 0;
	session->scheduler.in_flight_count = 0;
	session->scheduler.stop_found = 0;
	session->scheduler.stop_ttl = 0;
}

static void	init_output(t_session *session) {
	session->output.line_started = 0;
	session->output.has_last_addr = 0;
	session->output.last_addr.sin_family = 0;
	session->output.last_addr.sin_port = 0;
	session->output.last_addr.sin_addr.s_addr = 0;
}

int	traceroute_prepare_session(t_traceroute_config *config,
		t_session *session, int *exit_status) {
	init_session_empty(session);
	session->total_probes = compute_total_probes(config);
	session->probe_capacity = compute_probe_capacity(config,
		session->total_probes);
	session->probes = malloc(sizeof(*session->probes)
		* session->probe_capacity);
	if (!session->probes)
	{
		printf("%s: allocation failed\n", config->cli.program_name);
		*exit_status = 1;
		return (0);
	}
	init_probes(session);
	init_scheduler(config, session);
	init_output(session);
	init_payload(config);
	return (1);
}

void	traceroute_cleanup_session(t_session *session) {
	free(session->probes);
	session->probes = NULL;
	session->probe_capacity = 0;
	session->total_probes = 0;
}


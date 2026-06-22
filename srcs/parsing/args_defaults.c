
#include "args.h"

void	traceroute_args_init(t_traceroute_config *config) {
	config->cli.program_name = NULL;
	config->cli.target = NULL;
	config->cli.help = 0;
	config->cli.no_dns = 0;
	config->cli.sim_queries = TRACEROUTE_DEFAULT_SIM_QUERIES;
	config->cli.probes_per_hop = TRACEROUTE_DEFAULT_PROBES;
	config->cli.max_hops = TRACEROUTE_DEFAULT_MAX_HOPS;
	config->cli.first_ttl = TRACEROUTE_DEFAULT_FIRST_TTL;
	config->cli.base_port = TRACEROUTE_DEFAULT_BASE_PORT;
	config->cli.send_wait = TRACEROUTE_DEFAULT_SEND_WAIT;
	config->cli.wait_time = TRACEROUTE_DEFAULT_WAIT_TIME;
	config->cli.wait_here = TRACEROUTE_DEFAULT_WAIT_HERE;
	config->cli.wait_near = TRACEROUTE_DEFAULT_WAIT_NEAR;
	config->cli.error = TRACEROUTE_ARGS_OK;
	config->cli.cli_error = TRACEROUTE_CLI_OK;
	config->cli.bad_arg = NULL;
	config->cli.bad_index = -1;
	config->target.addr.sin_family = 0;
	config->target.addr.sin_port = 0;
	config->target.addr.sin_addr.s_addr = 0;
	config->target.addr_len = 0;
	config->target.ip[0] = '\0';
	config->target.error = TRACEROUTE_RESOLVE_OK;
	config->target.gai_error = 0;
	config->socket.udp_fd = -1;
	config->socket.icmp_fd = -1;
	config->socket.error = 0;
	config->packet.payload_len = TRACEROUTE_DEFAULT_PAYLOAD_LEN;
}

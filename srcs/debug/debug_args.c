#include "debug.h"

#ifdef DEBUG_ARGS

void	debug_print_args(const t_traceroute_config *config) {
	printf("[debug][args]\n");
	printf("  program_name    = %s\n", config->cli.program_name);
	printf("  target          = %s\n", config->cli.target);
	printf("  no_dns          = %d\n", config->cli.no_dns);
	printf("  sim_queries     = %d\n", config->cli.sim_queries);
	printf("  probes_per_hop  = %d\n", config->cli.probes_per_hop);
	if (config->cli.has_source)
		printf("  source          = %s\n", config->cli.source);
	else
		printf("  source          = (none)\n");
	printf("  max_hops        = %d\n", config->cli.max_hops);
	printf("  first_ttl       = %d\n", config->cli.first_ttl);
	printf("  base_port       = %d\n", config->cli.base_port);
	printf("  send_wait       = %.3f\n", config->cli.send_wait);
}

#endif

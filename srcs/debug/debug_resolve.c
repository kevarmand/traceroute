#include "debug.h"

#ifdef DEBUG_ARGS

void	debug_print_resolve(const t_traceroute_config *config) {
	printf("[debug][resolve]\n");
	printf("  target = %s\n", config->cli.target);
	printf("  ip     = %s\n", config->target.ip);
}

#endif
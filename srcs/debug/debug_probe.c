#include "debug.h"

#ifdef DEBUG_ARGS

void	debug_print_probe(const t_traceroute_config *config) {
	(void)config;
	printf("[debug][probe]\n");
}

#endif
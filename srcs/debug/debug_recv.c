#include "debug.h"

#ifdef DEBUG_ARGS

void	debug_print_recv_packet(const t_traceroute_config *config) {
	(void)config;
	printf("[debug][recv]\n");
}

#endif
#include "debug.h"

#ifdef DEBUG_ARGS

void	debug_print_packet(const t_traceroute_config *config) {
	printf("[debug][packet]\n");
	printf("  payload_len = %lu\n", (unsigned long)config->packet.payload_len);
}

#endif
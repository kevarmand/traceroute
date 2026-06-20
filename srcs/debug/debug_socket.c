#include "debug.h"

#ifdef DEBUG_ARGS

void	debug_print_socket(const t_traceroute_config *config) {
	printf("[debug][socket]\n");
	printf("  udp_fd  = %d\n", config->socket.udp_fd);
	printf("  icmp_fd = %d\n", config->socket.icmp_fd);
	printf("  error   = %d\n", config->socket.error);
}

#endif
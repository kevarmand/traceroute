#include "ft_traceroute.h"

void	traceroute_print_header(const t_traceroute_config *config) {
	size_t	packet_size;

	packet_size = 20 + 8 + config->packet.payload_len;
	printf("traceroute to %s (%s), %d hops max, %lu byte packets\n",
		config->cli.target,
		config->target.ip,
		config->cli.max_hops,
		(unsigned long)packet_size);
}
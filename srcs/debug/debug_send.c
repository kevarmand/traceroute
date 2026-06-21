#include "debug.h"

#ifdef DEBUG_ARGS

static void	print_payload_char(unsigned char c) {
	if (c == '\\')
		printf("\\\\");
	else if (c == '"')
		printf("\\\"");
	else if (c >= 32 && c <= 126)
		printf("%c", c);
	else
		printf(".");
}

static void	print_payload(const unsigned char *payload, size_t length) {
	size_t	i;

	i = 0;
	while (i < length)
	{
		print_payload_char(payload[i]);
		i++;
	}
}

void	debug_print_send(const t_traceroute_config *config,
		const t_probe *probe, ssize_t sent_len) {
	printf("[debug][send]\n");
	printf("  ttl          = %d\n", probe->ttl);
	printf("  probe_index  = %d\n", probe->probe_index);
	printf("  port         = %u\n", (unsigned int)probe->port);
	printf("  payload_len  = %lu\n",
		(unsigned long)config->packet.payload_len);
	printf("  sent_len     = %ld\n", (long)sent_len);
	printf("  target       = %s\n", config->target.ip);
	printf("  payload      = \"");
	print_payload(config->packet.payload, config->packet.payload_len);
	printf("\"\n");
}

#endif

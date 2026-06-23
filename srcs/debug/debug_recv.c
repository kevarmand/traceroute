#include "debug.h"

#ifdef DEBUG_ARGS

static const char	*probe_state_name(t_probe_state state) {
	if (state == PROBE_FREE)
		return ("FREE");
	if (state == PROBE_SENT)
		return ("SENT");
	if (state == PROBE_REPLY_PENDING)
		return ("REPLY_PENDING");
	if (state == PROBE_TIMEOUT_PENDING)
		return ("TIMEOUT_PENDING");
	return ("UNKNOWN");
}

static void	print_hexdump_ascii(const unsigned char *buffer,
		ssize_t length, ssize_t offset) {
	ssize_t			 i;
	unsigned char	c;

	i = 0;
	printf(" |");
	while (i < 16 && offset + i < length)
	{
		c = buffer[offset + i];
		if (c >= 32 && c <= 126)
			printf("%c", c);
		else
			printf(".");
		i++;
	}
	printf("|\n");
}

static void	print_hexdump_line(const unsigned char *buffer,
		ssize_t length, ssize_t offset) {
	ssize_t	i;

	printf("  %04lx  ", (unsigned long)offset);
	i = 0;
	while (i < 16)
	{
		if (offset + i < length)
			printf("%02x ", (unsigned int)buffer[offset + i]);
		else
			printf("   ");
		if (i == 7)
			printf(" ");
		i++;
	}
	print_hexdump_ascii(buffer, length, offset);
}

static void	print_hexdump(const unsigned char *buffer, ssize_t length) {
	ssize_t	offset;

	offset = 0;
	while (offset < length)
	{
		print_hexdump_line(buffer, length, offset);
		offset += 16;
	}
}

static void	print_matched_probe(const t_probe *probe) {
	if (!probe)
	{
		printf("    matched      = no\n");
		return;
	}
	printf("    matched      = yes\n");
	printf("    probe_ttl    = %d\n", probe->ttl);
	printf("    probe_index  = %d\n", probe->probe_index);
	printf("    probe_port   = %u\n", (unsigned int)probe->port);
	printf("    probe_state  = %s\n", probe_state_name(probe->state));
}

void	debug_print_recv_packet(const t_traceroute_config *config) {
	(void)config;
	printf("[debug][recv]\n");
}

void	debug_print_recv(const t_traceroute_config *config,
		const t_icmp_reply *reply, const t_probe *probe,
		const unsigned char *buffer, ssize_t length) {
	printf("[debug][recv]\n");
	printf("  raw_len      = %ld\n", (long)length);
	printf("  hexdump:\n");
	print_hexdump(buffer, length);
	printf("  parsed:\n");
	printf("    target       = %s\n", config->target.ip);
	printf("    from         = %s\n", inet_ntoa(reply->from.sin_addr));
	printf("    type         = %u\n", (unsigned int)reply->icmp_type);
	printf("    code         = %u\n", (unsigned int)reply->icmp_code);
	printf("    original_dst = %s\n", inet_ntoa(reply->original_dst));
	printf("    udp_src_port = %u\n", (unsigned int)reply->source_port);
	printf("    udp_dst_port = %u\n", (unsigned int)reply->port);
	printf("  match:\n");
	print_matched_probe(probe);
}

void	debug_print_recv_ignored(const t_traceroute_config *config,
		const struct sockaddr_in *from, const unsigned char *buffer,
		ssize_t length, t_icmp_parse_status status) {
	printf("[debug][recv][ignored]\n");
	printf("  target       = %s\n", config->target.ip);
	printf("  from         = %s\n", inet_ntoa(from->sin_addr));
	printf("  length       = %ld\n", (long)length);
	printf("  reason       = %s\n",
		traceroute_icmp_parse_status_name(status));
	printf("  hexdump:\n");
	print_hexdump(buffer, length);
}

#endif

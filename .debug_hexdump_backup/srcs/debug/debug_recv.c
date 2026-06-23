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

static void	print_matched_probe(const t_probe *probe) {
	if (!probe)
	{
		printf("  matched      = no\n");
		return;
	}
	printf("  matched      = yes\n");
	printf("  probe_ttl    = %d\n", probe->ttl);
	printf("  probe_index  = %d\n", probe->probe_index);
	printf("  probe_port   = %u\n", (unsigned int)probe->port);
	printf("  probe_state  = %s\n", probe_state_name(probe->state));
}

void	debug_print_recv_packet(const t_traceroute_config *config) {
	(void)config;
	printf("[debug][recv]\n");
}

void	debug_print_recv(const t_traceroute_config *config,
		const t_icmp_reply *reply, const t_probe *probe) {
	printf("[debug][recv]\n");
	printf("  target       = %s\n", config->target.ip);
	printf("  from         = %s\n", inet_ntoa(reply->from.sin_addr));
	printf("  type         = %u\n", (unsigned int)reply->icmp_type);
	printf("  code         = %u\n", (unsigned int)reply->icmp_code);
	printf("  original_dst = %s\n", inet_ntoa(reply->original_dst));
	printf("  udp_src_port = %u\n", (unsigned int)reply->source_port);
	printf("  udp_dst_port = %u\n", (unsigned int)reply->port);
	print_matched_probe(probe);
}

void	debug_print_recv_ignored(const t_traceroute_config *config,
		const struct sockaddr_in *from, ssize_t length,
		t_icmp_parse_status status) {
	printf("[debug][recv][ignored]\n");
	printf("  target       = %s\n", config->target.ip);
	printf("  from         = %s\n", inet_ntoa(from->sin_addr));
	printf("  length       = %ld\n", (long)length);
	printf("  reason       = %s\n",
		traceroute_icmp_parse_status_name(status));
}

#endif

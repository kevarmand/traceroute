#include "packet.h"

const char	*traceroute_icmp_parse_status_name(
		t_icmp_parse_status status) {
	if (status == ICMP_PARSE_OK)
		return ("ok");
	if (status == ICMP_PARSE_SHORT_OUTER_IP)
		return ("short outer ip");
	if (status == ICMP_PARSE_BAD_OUTER_IP)
		return ("bad outer ip");
	if (status == ICMP_PARSE_SHORT_ICMP)
		return ("short icmp");
	if (status == ICMP_PARSE_IGNORED_TYPE)
		return ("ignored icmp type");
	if (status == ICMP_PARSE_SHORT_INNER_IP)
		return ("short inner ip");
	if (status == ICMP_PARSE_BAD_INNER_IP)
		return ("bad inner ip");
	if (status == ICMP_PARSE_NOT_UDP)
		return ("inner protocol is not udp");
	if (status == ICMP_PARSE_SHORT_UDP)
		return ("short udp");
	return ("unknown");
}

static t_icmp_parse_status	get_outer_ip_header_length(
		const unsigned char *buffer, ssize_t length, int *header_length) {
	const struct iphdr	*ip;

	if (length < (ssize_t)sizeof(struct iphdr))
		return (ICMP_PARSE_SHORT_OUTER_IP);
	ip = (const struct iphdr *)buffer;
	*header_length = ip->ihl * 4;
	if (ip->version != 4 || *header_length < (int)sizeof(struct iphdr))
		return (ICMP_PARSE_BAD_OUTER_IP);
	if (length < *header_length)
		return (ICMP_PARSE_SHORT_OUTER_IP);
	return (ICMP_PARSE_OK);
}

static t_icmp_parse_status	get_inner_ip_header_length(
		const unsigned char *buffer, ssize_t length, int *header_length) {
	const struct iphdr	*ip;

	if (length < (ssize_t)sizeof(struct iphdr))
		return (ICMP_PARSE_SHORT_INNER_IP);
	ip = (const struct iphdr *)buffer;
	*header_length = ip->ihl * 4;
	if (ip->version != 4 || *header_length < (int)sizeof(struct iphdr))
		return (ICMP_PARSE_BAD_INNER_IP);
	if (length < *header_length)
		return (ICMP_PARSE_SHORT_INNER_IP);
	return (ICMP_PARSE_OK);
}

static t_icmp_parse_status	parse_original_udp(
		const unsigned char *buffer, ssize_t length, t_icmp_reply *reply) {
	const struct iphdr	*ip;
	const struct udphdr	*udp;
	int						header_length;
	t_icmp_parse_status	status;

	status = get_inner_ip_header_length(buffer, length, &header_length);
	if (status != ICMP_PARSE_OK)
		return (status);
	ip = (const struct iphdr *)buffer;
	if (ip->protocol != IPPROTO_UDP)
		return (ICMP_PARSE_NOT_UDP);
	if (length < header_length + (ssize_t)sizeof(struct udphdr))
		return (ICMP_PARSE_SHORT_UDP);
	udp = (const struct udphdr *)(buffer + header_length);
	reply->original_dst.s_addr = ip->daddr;
	reply->source_port = ntohs(udp->source);
	reply->port = ntohs(udp->dest);
	return (ICMP_PARSE_OK);
}

static int	is_traceroute_icmp(unsigned char type) {
	if (type == ICMP_TIME_EXCEEDED)
		return (1);
	if (type == ICMP_DEST_UNREACH)
		return (1);
	return (0);
}

t_icmp_parse_status	traceroute_parse_icmp_reply(
		const unsigned char *buffer, ssize_t length,
		const struct sockaddr_in *from, t_icmp_reply *reply) {
	const struct icmphdr	*icmp;
	const unsigned char	*original;
	ssize_t				remaining;
	int					outer_ihl;
	t_icmp_parse_status	status;

	status = get_outer_ip_header_length(buffer, length, &outer_ihl);
	if (status != ICMP_PARSE_OK)
		return (status);
	if (length < outer_ihl + (ssize_t)sizeof(struct icmphdr))
		return (ICMP_PARSE_SHORT_ICMP);
	icmp = (const struct icmphdr *)(buffer + outer_ihl);
	reply->from = *from;
	reply->icmp_type = icmp->type;
	reply->icmp_code = icmp->code;
	if (!is_traceroute_icmp(icmp->type))
		return (ICMP_PARSE_IGNORED_TYPE);
	original = buffer + outer_ihl + sizeof(struct icmphdr);
	remaining = length - outer_ihl - sizeof(struct icmphdr);
	return (parse_original_udp(original, remaining, reply));
}

#include "packet.h"

static int	get_ip_header_length(const unsigned char *buffer,
		ssize_t length) {
	const struct iphdr	*ip;
	int						header_length;

	if (length < (ssize_t)sizeof(struct iphdr))
		return (-1);
	ip = (const struct iphdr *)buffer;
	header_length = ip->ihl * 4;
	if (header_length < (int)sizeof(struct iphdr))
		return (-1);
	if (length < header_length)
		return (-1);
	return (header_length);
}

static int	parse_original_udp_port(const unsigned char *buffer,
		ssize_t length, unsigned short *port) {
	const struct iphdr	*ip;
	const struct udphdr	*udp;
	int						header_length;

	header_length = get_ip_header_length(buffer, length);
	if (header_length < 0)
		return (0);
	ip = (const struct iphdr *)buffer;
	if (ip->protocol != IPPROTO_UDP)
		return (0);
	if (length < header_length + (ssize_t)sizeof(struct udphdr))
		return (0);
	udp = (const struct udphdr *)(buffer + header_length);
	*port = ntohs(udp->dest);
	return (1);
}

static int	is_traceroute_icmp(unsigned char type) {
	if (type == ICMP_TIME_EXCEEDED)
		return (1);
	if (type == ICMP_DEST_UNREACH)
		return (1);
	return (0);
}

int	traceroute_parse_icmp_reply(const unsigned char *buffer,
		ssize_t length, const struct sockaddr_in *from, t_icmp_reply *reply) {
	const struct icmphdr	*icmp;
	const unsigned char	*original;
	ssize_t				remaining;
	int					outer_ihl;

	outer_ihl = get_ip_header_length(buffer, length);
	if (outer_ihl < 0)
		return (0);
	if (length < outer_ihl + (ssize_t)sizeof(struct icmphdr))
		return (0);
	icmp = (const struct icmphdr *)(buffer + outer_ihl);
	if (!is_traceroute_icmp(icmp->type))
		return (0);
	original = buffer + outer_ihl + sizeof(struct icmphdr);
	remaining = length - outer_ihl - sizeof(struct icmphdr);
	if (!parse_original_udp_port(original, remaining, &reply->port))
		return (0);
	reply->from = *from;
	reply->icmp_type = icmp->type;
	reply->icmp_code = icmp->code;
	return (1);
}

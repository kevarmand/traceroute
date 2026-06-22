#ifndef PACKET_H
# define PACKET_H

# include "ft_traceroute.h"

typedef enum e_icmp_parse_status {
	ICMP_PARSE_OK,
	ICMP_PARSE_SHORT_OUTER_IP,
	ICMP_PARSE_BAD_OUTER_IP,
	ICMP_PARSE_SHORT_ICMP,
	ICMP_PARSE_IGNORED_TYPE,
	ICMP_PARSE_SHORT_INNER_IP,
	ICMP_PARSE_BAD_INNER_IP,
	ICMP_PARSE_NOT_UDP,
	ICMP_PARSE_SHORT_UDP
}	t_icmp_parse_status;

typedef struct s_icmp_reply {
	struct sockaddr_in	from;
	struct in_addr		original_dst;
	unsigned short		source_port;
	unsigned short		port;
	unsigned char		icmp_type;
	unsigned char		icmp_code;
}	t_icmp_reply;

t_icmp_parse_status	traceroute_parse_icmp_reply(const unsigned char *buffer,
		ssize_t length, const struct sockaddr_in *from, t_icmp_reply *reply);
const char			*traceroute_icmp_parse_status_name(
		t_icmp_parse_status status);

#endif

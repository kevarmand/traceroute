#ifndef PACKET_H
# define PACKET_H

# include "ft_traceroute.h"

typedef struct s_icmp_reply {
	struct sockaddr_in	from;
	unsigned short		port;
	unsigned char		icmp_type;
	unsigned char		icmp_code;
}	t_icmp_reply;

int	traceroute_parse_icmp_reply(const unsigned char *buffer,
		ssize_t length, const struct sockaddr_in *from, t_icmp_reply *reply);

#endif

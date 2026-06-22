#ifndef DEBUG_H
# define DEBUG_H

# include "session.h"
# include "packet.h"

# ifdef DEBUG_ARGS

void	debug_print_args(const t_traceroute_config *config);
void	debug_print_resolve(const t_traceroute_config *config);
void	debug_print_socket(const t_traceroute_config *config);
void	debug_print_packet(const t_traceroute_config *config);
void	debug_print_probe(const t_traceroute_config *config);
void	debug_print_recv_packet(const t_traceroute_config *config);
void	debug_print_send(const t_traceroute_config *config,
			const t_probe *probe, ssize_t sent_len);
void	debug_print_recv(const t_traceroute_config *config,
			const t_icmp_reply *reply, const t_probe *probe);
void	debug_print_recv_ignored(const t_traceroute_config *config,
			const struct sockaddr_in *from, ssize_t length,
			t_icmp_parse_status status);

#  define DEBUG_ARGS_PARSE(config) debug_print_args(config)
#  define DEBUG_RESOLVE(config) debug_print_resolve(config)
#  define DEBUG_SOCKET(config) debug_print_socket(config)
#  define DEBUG_PACKET(config) debug_print_packet(config)
#  define DEBUG_PROBE(config) debug_print_probe(config)
#  define DEBUG_RECV_PACKET(config) debug_print_recv_packet(config)
#  define DEBUG_SEND(config, probe, sent_len) \
	debug_print_send(config, probe, sent_len)
#  define DEBUG_RECV(config, reply, probe) \
	debug_print_recv(config, reply, probe)
#  define DEBUG_RECV_IGNORED(config, from, length, status) \
	debug_print_recv_ignored(config, from, length, status)

# else

#  define DEBUG_ARGS_PARSE(config) ((void)(config))
#  define DEBUG_RESOLVE(config) ((void)(config))
#  define DEBUG_SOCKET(config) ((void)(config))
#  define DEBUG_PACKET(config) ((void)(config))
#  define DEBUG_PROBE(config) ((void)(config))
#  define DEBUG_RECV_PACKET(config) ((void)(config))
#  define DEBUG_SEND(config, probe, sent_len) \
	((void)(config), (void)(probe), (void)(sent_len))
#  define DEBUG_RECV(config, reply, probe) \
	((void)(config), (void)(reply), (void)(probe))
#  define DEBUG_RECV_IGNORED(config, from, length, status) \
	((void)(config), (void)(from), (void)(length), (void)(status))

# endif

#endif

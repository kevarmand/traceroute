#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include <arpa/inet.h>
# include <errno.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
# include "traceroute_config.h"

int			traceroute_prepare_cli(int argc, char **argv,
					t_traceroute_config *config, int *exit_status);
int			traceroute_prepare_target(t_traceroute_config *config,
					int *exit_status);
int			traceroute_prepare_sockets(t_traceroute_config *config,
					int *exit_status);

const char	*traceroute_args_error_name(int error);
const char	*traceroute_cli_error_name(int error);

int			traceroute_resolve_target(t_traceroute_config *config);
const char	*traceroute_resolve_error_name(int error);

int			traceroute_open_sockets(t_traceroute_config *config);
int			traceroute_set_socket_ttl(t_traceroute_config *config, int ttl);
void		traceroute_close_sockets(t_traceroute_config *config);

void		traceroute_print_error(const t_traceroute_config *config);
void		traceroute_print_header(const t_traceroute_config *config);
void		traceroute_print_usage(const char *program_name);

#endif

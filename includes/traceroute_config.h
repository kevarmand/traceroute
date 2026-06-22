
#ifndef TRACEROUTE_CONFIG_H
# define TRACEROUTE_CONFIG_H

# include <arpa/inet.h>
# include <netinet/in.h>
# include <stddef.h>
# include <sys/socket.h>

typedef enum e_traceroute_args_error {
	TRACEROUTE_ARGS_OK = 0,
	TRACEROUTE_ARGS_ALLOC,
	TRACEROUTE_ARGS_CLI,
	TRACEROUTE_ARGS_MISSING_TARGET,
	TRACEROUTE_ARGS_TOO_MANY_TARGETS,
	TRACEROUTE_ARGS_INVALID_RANGE
}	t_traceroute_args_error;

typedef enum e_traceroute_cli_error {
	TRACEROUTE_CLI_OK = 0,
	TRACEROUTE_CLI_UNKNOWN_OPTION,
	TRACEROUTE_CLI_TOO_MANY_OPTIONS,
	TRACEROUTE_CLI_TOO_MANY_ARGS,
	TRACEROUTE_CLI_MISSING_VALUE,
	TRACEROUTE_CLI_INVALID_VALUE,
	TRACEROUTE_CLI_UNKNOWN_ERROR
}	t_traceroute_cli_error;

typedef enum e_traceroute_resolve_error {
	TRACEROUTE_RESOLVE_OK = 0,
	TRACEROUTE_RESOLVE_GAI,
	TRACEROUTE_RESOLVE_NO_INET
}	t_traceroute_resolve_error;

typedef struct s_traceroute_cli {
	const char	*program_name;
	const char	*target;
	int			help;
	int			no_dns;
	int			sim_queries;
	int			probes_per_hop;
	int			max_hops;
	int			first_ttl;
	int			base_port;
	double		send_wait;
	double		wait_time;
	double		wait_here;
	double		wait_near;
	int			error;
	int			cli_error;
	const char	*bad_arg;
	int			bad_index;
}	t_traceroute_cli;

typedef struct s_traceroute_target {
	struct sockaddr_in	addr;
	socklen_t			addr_len;
	char				ip[INET_ADDRSTRLEN];
	int					error;
	int					gai_error;
}	t_traceroute_target;

typedef struct s_traceroute_socket {
	int	udp_fd;
	int	icmp_fd;
	int	error;
}	t_traceroute_socket;

typedef struct s_traceroute_packet {
	unsigned char	payload[64];
	size_t			payload_len;
}	t_traceroute_packet;

typedef struct s_traceroute_config {
	t_traceroute_cli		cli;
	t_traceroute_target	target;
	t_traceroute_socket	socket;
	t_traceroute_packet	packet;
}	t_traceroute_config;

#endif


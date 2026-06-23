#ifndef ARGS_H
# define ARGS_H

# include "ft_traceroute.h"
# include "arg_parser.h"

# define TRACEROUTE_DEFAULT_MAX_HOPS 30
# define TRACEROUTE_DEFAULT_FIRST_TTL 1
# define TRACEROUTE_DEFAULT_PROBES 3
# define TRACEROUTE_DEFAULT_SIM_QUERIES 16
# define TRACEROUTE_DEFAULT_BASE_PORT 33434
# define TRACEROUTE_DEFAULT_SEND_WAIT 0.0
# define TRACEROUTE_DEFAULT_WAIT_TIME 5.0
# define TRACEROUTE_DEFAULT_WAIT_HERE 3.0
# define TRACEROUTE_DEFAULT_WAIT_NEAR 10.0
# define TRACEROUTE_DEFAULT_PAYLOAD_LEN 32

# define TRACEROUTE_MAX_TTL 255
# define TRACEROUTE_MAX_PROBES 10
# define TRACEROUTE_MAX_SIM_QUERIES 256
# define TRACEROUTE_MAX_PORT 65535

typedef enum e_traceroute_opt {
	TRACEROUTE_OPT_HELP = 1,
	TRACEROUTE_OPT_NO_DNS,
	TRACEROUTE_OPT_SIM_QUERIES,
	TRACEROUTE_OPT_PROBES,
	TRACEROUTE_OPT_MAX_HOPS,
	TRACEROUTE_OPT_FIRST_TTL,
	TRACEROUTE_OPT_BASE_PORT,
	TRACEROUTE_OPT_SEND_WAIT,
	TRACEROUTE_OPT_WAIT_TIME
}t_traceroute_opt;

void			traceroute_args_init(t_traceroute_config *config);
int				traceroute_args_parse(int argc, char **argv,
					t_traceroute_config *config);
const char		*traceroute_args_error_name(int error);
const char		*traceroute_cli_error_name(int error);

const t_arg_spec	*traceroute_args_specs(void);
int				traceroute_apply_options(t_traceroute_config *config,
					const t_arg_result *result);
int				traceroute_validate_args(t_traceroute_config *config,
					const t_arg_result *result);
int				traceroute_set_value_error(t_traceroute_config *config,
					const t_arg_option *option);
int				traceroute_set_range_error(t_traceroute_config *config);
int				traceroute_set_cli_error(t_traceroute_config *config,
					const t_arg_result *result);
int				traceroute_parse_int_value(const char *str, int min, int max,
					int *out);
int				traceroute_parse_send_wait_option(t_traceroute_config *config,
					const t_arg_option *option);
int				traceroute_parse_wait_option(t_traceroute_config *config,
					const t_arg_option *option);

#endif

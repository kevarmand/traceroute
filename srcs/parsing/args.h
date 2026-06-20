#ifndef ARGS_H
# define ARGS_H

# include "ft_traceroute.h"

void		traceroute_args_init(t_traceroute_config *config);
int			traceroute_args_parse(int argc, char **argv,
				t_traceroute_config *config);
const char	*traceroute_args_error_name(int error);

#endif

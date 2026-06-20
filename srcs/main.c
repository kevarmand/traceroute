#include "ft_traceroute.h"
#include "args.h"
#include "debug.h"

int	main(int argc, char **argv) {
	t_traceroute_config	config;
	int					error;

	error = traceroute_args_parse(argc, argv, &config);
	if (error != TRACEROUTE_ARGS_OK)
	{
		traceroute_print_error(&config);
		return (1);
	}
	if (config.cli.help)
	{
		traceroute_print_usage(config.cli.program_name);
		return (0);
	}
	DEBUG_ARGS_PARSE(&config);
	error = traceroute_resolve_target(&config);
	if (error != TRACEROUTE_RESOLVE_OK)
	{
		traceroute_print_error(&config);
		return (1);
	}
	DEBUG_RESOLVE(&config);
	traceroute_print_header(&config);
	return (0);
}
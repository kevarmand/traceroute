
#include "args.h"

int	traceroute_validate_args(t_traceroute_config *config,
		const t_arg_result *result) {
	if (config->cli.help)
		return (TRACEROUTE_ARGS_OK);
	if (result->arg_count == 0)
	{
		config->cli.error = TRACEROUTE_ARGS_MISSING_TARGET;
		return (config->cli.error);
	}
	if (result->arg_count > 1)
	{
		config->cli.error = TRACEROUTE_ARGS_TOO_MANY_TARGETS;
		config->cli.bad_arg = result->args[1];
		config->cli.bad_index = -1;
		return (config->cli.error);
	}
	if (config->cli.first_ttl > config->cli.max_hops)
		return (traceroute_set_range_error(config));
	config->cli.target = result->args[0];
	return (TRACEROUTE_ARGS_OK);
}

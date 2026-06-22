
#include "args.h"

int	traceroute_set_value_error(t_traceroute_config *config,
		const t_arg_option *option) {
	config->cli.error = TRACEROUTE_ARGS_CLI;
	config->cli.cli_error = TRACEROUTE_CLI_INVALID_VALUE;
	config->cli.bad_arg = option->value;
	config->cli.bad_index = option->argv_index;
	return (config->cli.error);
}

int	traceroute_set_range_error(t_traceroute_config *config) {
	config->cli.error = TRACEROUTE_ARGS_INVALID_RANGE;
	config->cli.bad_arg = NULL;
	config->cli.bad_index = -1;
	return (config->cli.error);
}

static int	map_cli_error(int error) {
	if (error == ARG_ERR_UNKNOWN_OPTION)
		return (TRACEROUTE_CLI_UNKNOWN_OPTION);
	if (error == ARG_ERR_TOO_MANY_OPTIONS)
		return (TRACEROUTE_CLI_TOO_MANY_OPTIONS);
	if (error == ARG_ERR_TOO_MANY_ARGS)
		return (TRACEROUTE_CLI_TOO_MANY_ARGS);
	if (error == ARG_ERR_MISSING_VALUE)
		return (TRACEROUTE_CLI_MISSING_VALUE);
	if (error == ARG_ERR_INVALID_VALUE)
		return (TRACEROUTE_CLI_INVALID_VALUE);
	return (TRACEROUTE_CLI_UNKNOWN_ERROR);
}

int	traceroute_set_cli_error(t_traceroute_config *config,
		const t_arg_result *result) {
	config->cli.error = TRACEROUTE_ARGS_CLI;
	config->cli.cli_error = map_cli_error(result->error);
	config->cli.bad_arg = result->bad_arg;
	config->cli.bad_index = result->bad_index;
	return (config->cli.error);
}

const char	*traceroute_args_error_name(int error) {
	if (error == TRACEROUTE_ARGS_OK)
		return ("ok");
	if (error == TRACEROUTE_ARGS_ALLOC)
		return ("allocation failed");
	if (error == TRACEROUTE_ARGS_CLI)
		return ("command line parsing error");
	if (error == TRACEROUTE_ARGS_MISSING_TARGET)
		return ("missing destination");
	if (error == TRACEROUTE_ARGS_TOO_MANY_TARGETS)
		return ("too many destinations");
	if (error == TRACEROUTE_ARGS_INVALID_RANGE)
		return ("invalid option range");
	return ("unknown traceroute argument error");
}

const char	*traceroute_cli_error_name(int error) {
	if (error == TRACEROUTE_CLI_OK)
		return ("ok");
	if (error == TRACEROUTE_CLI_UNKNOWN_OPTION)
		return ("unknown option");
	if (error == TRACEROUTE_CLI_TOO_MANY_OPTIONS)
		return ("too many options");
	if (error == TRACEROUTE_CLI_TOO_MANY_ARGS)
		return ("too many arguments");
	if (error == TRACEROUTE_CLI_MISSING_VALUE)
		return ("missing option value");
	if (error == TRACEROUTE_CLI_INVALID_VALUE)
		return ("invalid option value");
	return ("unknown command line parsing error");
}

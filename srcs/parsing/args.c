
#include "args.h"

int	traceroute_args_parse(int argc, char **argv,
		t_traceroute_config *config) {
	t_arg_result	result;
	int				error;

	traceroute_args_init(config);
	config->cli.program_name = argv[0];
	error = arg_parser_result_init(&result, argc);
	if (error != ARG_OK)
	{
		config->cli.error = TRACEROUTE_ARGS_ALLOC;
		return (config->cli.error);
	}
	error = arg_parser_parse(argc, argv, traceroute_args_specs(), &result);
	if (error != ARG_OK)
	{
		traceroute_set_cli_error(config, &result);
		arg_parser_result_clear(&result);
		return (config->cli.error);
	}
	error = traceroute_apply_options(config, &result);
	if (error == TRACEROUTE_ARGS_OK)
		error = traceroute_validate_args(config, &result);
	arg_parser_result_clear(&result);
	return (error);
}

int	traceroute_prepare_cli(int argc, char **argv,
		t_traceroute_config *config, int *exit_status) {
	int	error;

	error = traceroute_args_parse(argc, argv, config);
	if (error != TRACEROUTE_ARGS_OK)
	{
		traceroute_print_error(config);
		*exit_status = 1;
		return (0);
	}
	if (config->cli.help)
	{
		traceroute_print_usage(config->cli.program_name);
		*exit_status = 0;
		return (0);
	}
	return (1);
}

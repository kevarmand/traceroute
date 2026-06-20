#include "ft_traceroute.h"
#include "arg_parser.h"
#include "args.h"

static void	print_cli_error(const t_traceroute_config *config) {
	if (config->cli.cli_error == ARG_ERR_UNKNOWN_OPTION)
	{
		printf("Bad option `%s' (argc %d)\n",
			config->cli.bad_arg, config->cli.bad_index);
		return;
	}
	if (config->cli.cli_error == ARG_ERR_MISSING_VALUE)
	{
		printf("Option `%s' requires an argument (argc %d)\n",
			config->cli.bad_arg, config->cli.bad_index);
		return;
	}
	if (config->cli.cli_error == ARG_ERR_INVALID_VALUE)
	{
		printf("Bad value `%s' (argc %d)\n",
			config->cli.bad_arg, config->cli.bad_index);
		return;
	}
	printf("%s\n", arg_parser_error_name(config->cli.cli_error));
}

static void	print_resolve_error(const t_traceroute_config *config) {
	if (config->target.error == TRACEROUTE_RESOLVE_GAI)
	{
		printf("%s: %s\n", config->cli.target,
			gai_strerror(config->target.gai_error));
		printf("Cannot handle \"host\" cmdline arg `%s'\n",
			config->cli.target);
		return;
	}
	printf("%s: %s\n", config->cli.target,
		traceroute_resolve_error_name(config->target.error));
}

void	traceroute_print_error(const t_traceroute_config *config) {
	if (config->cli.error == TRACEROUTE_ARGS_CLI)
	{
		print_cli_error(config);
		return;
	}
	if (config->cli.error == TRACEROUTE_ARGS_MISSING_TARGET)
	{
		traceroute_print_usage(config->cli.program_name);
		return;
	}
	if (config->cli.error != TRACEROUTE_ARGS_OK)
	{
		printf("%s\n", traceroute_args_error_name(config->cli.error));
		return;
	}
	if (config->target.error != TRACEROUTE_RESOLVE_OK)
		print_resolve_error(config);
}

#include "args.h"

static const t_arg_spec	g_traceroute_specs[] = {
	{TRACEROUTE_OPT_HELP, 'h', "help", 0},
	{TRACEROUTE_OPT_NO_DNS, 'n', NULL, 0},
	{TRACEROUTE_OPT_SIM_QUERIES, 'N', "sim-queries", 1},
	{TRACEROUTE_OPT_PROBES, 'q', "queries", 1},
	{TRACEROUTE_OPT_MAX_HOPS, 'm', "max-hops", 1},
	{TRACEROUTE_OPT_FIRST_TTL, 'f', "first", 1},
	{TRACEROUTE_OPT_BASE_PORT, 'p', "port", 1},
	{TRACEROUTE_OPT_SEND_WAIT, 'z', "sendwait", 1},
	{TRACEROUTE_OPT_WAIT_TIME, 'w', "wait", 1},
	{ARG_ID_END, 0, NULL, 0}
};

const t_arg_spec	*traceroute_args_specs(void) {
	return (g_traceroute_specs);
}

static int	apply_traceroute_option(t_traceroute_config *config,
		const t_arg_option *option) {
	if (option->id == TRACEROUTE_OPT_HELP)
		config->cli.help = 1;
	else if (option->id == TRACEROUTE_OPT_NO_DNS)
		config->cli.no_dns = 1;
	else if (option->id == TRACEROUTE_OPT_SIM_QUERIES
		&& traceroute_parse_int_value(option->value, 1,
			TRACEROUTE_MAX_SIM_QUERIES, &config->cli.sim_queries))
		return (traceroute_set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_PROBES
		&& traceroute_parse_int_value(option->value, 1,
			TRACEROUTE_MAX_PROBES, &config->cli.probes_per_hop))
		return (traceroute_set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_MAX_HOPS
		&& traceroute_parse_int_value(option->value, 1,
			TRACEROUTE_MAX_TTL, &config->cli.max_hops))
		return (traceroute_set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_FIRST_TTL
		&& traceroute_parse_int_value(option->value, 1,
			TRACEROUTE_MAX_TTL, &config->cli.first_ttl))
		return (traceroute_set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_BASE_PORT
		&& traceroute_parse_int_value(option->value, 1,
			TRACEROUTE_MAX_PORT, &config->cli.base_port))
		return (traceroute_set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_SEND_WAIT)
		return (traceroute_parse_send_wait_option(config, option));
	else if (option->id == TRACEROUTE_OPT_WAIT_TIME)
		return (traceroute_parse_wait_option(config, option));
	return (TRACEROUTE_ARGS_OK);
}

int	traceroute_apply_options(t_traceroute_config *config,
		const t_arg_result *result) {
	int	i;
	int	error;

	i = 0;
	while (i < result->option_count)
	{
		error = apply_traceroute_option(config, &result->options[i]);
		if (error != TRACEROUTE_ARGS_OK)
			return (error);
		i++;
	}
	return (TRACEROUTE_ARGS_OK);
}

#include "args.h"
#include "arg_parser.h"
#include <stddef.h>

#define TRACEROUTE_DEFAULT_MAX_HOPS 30
#define TRACEROUTE_DEFAULT_FIRST_TTL 1
#define TRACEROUTE_DEFAULT_PROBES 3
#define TRACEROUTE_DEFAULT_SIM_QUERIES 16
#define TRACEROUTE_DEFAULT_BASE_PORT 33434
#define TRACEROUTE_DEFAULT_SEND_WAIT 0.0
#define TRACEROUTE_DEFAULT_WAIT_TIME 5.0
#define TRACEROUTE_DEFAULT_PAYLOAD_LEN 32

#define TRACEROUTE_MAX_TTL 255
#define TRACEROUTE_MAX_PROBES 1024
#define TRACEROUTE_MAX_SIM_QUERIES 65535
#define TRACEROUTE_MAX_PORT 65535

typedef enum e_traceroute_opt {
	TRACEROUTE_OPT_HELP = 1,
	TRACEROUTE_OPT_NO_DNS,
	TRACEROUTE_OPT_SIM_QUERIES,
	TRACEROUTE_OPT_PROBES,
	TRACEROUTE_OPT_SOURCE,
	TRACEROUTE_OPT_MAX_HOPS,
	TRACEROUTE_OPT_FIRST_TTL,
	TRACEROUTE_OPT_BASE_PORT,
	TRACEROUTE_OPT_SEND_WAIT,
	TRACEROUTE_OPT_WAIT_TIME
}	t_traceroute_opt;

static const t_arg_spec	g_traceroute_specs[] = {
	{TRACEROUTE_OPT_HELP, 'h', "help", 0},
	{TRACEROUTE_OPT_NO_DNS, 'n', NULL, 0},
	{TRACEROUTE_OPT_SIM_QUERIES, 'N', "sim-queries", 1},
	{TRACEROUTE_OPT_PROBES, 'q', "queries", 1},
	{TRACEROUTE_OPT_SOURCE, 's', "source", 1},
	{TRACEROUTE_OPT_MAX_HOPS, 'm', "max-hops", 1},
	{TRACEROUTE_OPT_FIRST_TTL, 'f', "first", 1},
	{TRACEROUTE_OPT_BASE_PORT, 'p', "port", 1},
	{TRACEROUTE_OPT_SEND_WAIT, 'z', "sendwait", 1},
	{TRACEROUTE_OPT_WAIT_TIME, 'w', "wait", 1},
	{ARG_ID_END, 0, NULL, 0}
};

void	traceroute_args_init(t_traceroute_config *config) {
	config->cli.program_name = NULL;
	config->cli.target = NULL;
	config->cli.source = NULL;
	config->cli.help = 0;
	config->cli.no_dns = 0;
	config->cli.has_source = 0;
	config->cli.sim_queries = TRACEROUTE_DEFAULT_SIM_QUERIES;
	config->cli.probes_per_hop = TRACEROUTE_DEFAULT_PROBES;
	config->cli.max_hops = TRACEROUTE_DEFAULT_MAX_HOPS;
	config->cli.first_ttl = TRACEROUTE_DEFAULT_FIRST_TTL;
	config->cli.base_port = TRACEROUTE_DEFAULT_BASE_PORT;
	config->cli.send_wait = TRACEROUTE_DEFAULT_SEND_WAIT;
	config->cli.wait_time = TRACEROUTE_DEFAULT_WAIT_TIME;
	config->cli.error = TRACEROUTE_ARGS_OK;
	config->cli.cli_error = TRACEROUTE_CLI_OK;
	config->cli.bad_arg = NULL;
	config->cli.bad_index = -1;
	config->target.addr.sin_family = 0;
	config->target.addr.sin_port = 0;
	config->target.addr.sin_addr.s_addr = 0;
	config->target.addr_len = 0;
	config->target.ip[0] = '\0';
	config->target.error = TRACEROUTE_RESOLVE_OK;
	config->target.gai_error = 0;
	config->socket.udp_fd = -1;
	config->socket.icmp_fd = -1;
	config->socket.error = 0;
	config->packet.payload_len = TRACEROUTE_DEFAULT_PAYLOAD_LEN;
}

static int	set_value_error(t_traceroute_config *config,
		const t_arg_option *option) {
	config->cli.error = TRACEROUTE_ARGS_CLI;
	config->cli.cli_error = TRACEROUTE_CLI_INVALID_VALUE;
	config->cli.bad_arg = option->value;
	config->cli.bad_index = option->argv_index;
	return (config->cli.error);
}

static int	set_range_error(t_traceroute_config *config) {
	config->cli.error = TRACEROUTE_ARGS_INVALID_RANGE;
	config->cli.bad_arg = NULL;
	config->cli.bad_index = -1;
	return (config->cli.error);
}

static int	parse_int_value(const char *str, int min, int max, int *out) {
	int	i;
	int	value;
	int	digit;

	i = 0;
	value = 0;
	if (!str[0])
		return (-1);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (-1);
		digit = str[i] - '0';
		if (value > (max - digit) / 10)
			return (-1);
		value = value * 10 + digit;
		i++;
	}
	if (value < min)
		return (-1);
	*out = value;
	return (0);
}

static int	parse_double_value(const char *str, double min, double *out) {
	int		i;
	int		has_digit;
	double	value;
	double	divisor;

	i = 0;
	has_digit = 0;
	value = 0.0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		has_digit = 1;
		value = value * 10.0 + (str[i] - '0');
		i++;
	}
	if (str[i] == '.')
	{
		i++;
		divisor = 10.0;
		while (str[i] >= '0' && str[i] <= '9')
		{
			has_digit = 1;
			value += (str[i] - '0') / divisor;
			divisor *= 10.0;
			i++;
		}
	}
	if (!has_digit || str[i] || value < min)
		return (-1);
	*out = value;
	return (0);
}

static int	apply_traceroute_option(t_traceroute_config *config,
		const t_arg_option *option) {
	if (option->id == TRACEROUTE_OPT_HELP)
		config->cli.help = 1;
	else if (option->id == TRACEROUTE_OPT_NO_DNS)
		config->cli.no_dns = 1;
	else if (option->id == TRACEROUTE_OPT_SIM_QUERIES
		&& parse_int_value(option->value, 1, TRACEROUTE_MAX_SIM_QUERIES,
			&config->cli.sim_queries))
		return (set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_PROBES
		&& parse_int_value(option->value, 1, TRACEROUTE_MAX_PROBES,
			&config->cli.probes_per_hop))
		return (set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_SOURCE)
	{
		config->cli.has_source = 1;
		config->cli.source = option->value;
	}
	else if (option->id == TRACEROUTE_OPT_MAX_HOPS
		&& parse_int_value(option->value, 1, TRACEROUTE_MAX_TTL,
			&config->cli.max_hops))
		return (set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_FIRST_TTL
		&& parse_int_value(option->value, 1, TRACEROUTE_MAX_TTL,
			&config->cli.first_ttl))
		return (set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_BASE_PORT
		&& parse_int_value(option->value, 1, TRACEROUTE_MAX_PORT,
			&config->cli.base_port))
		return (set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_SEND_WAIT
		&& parse_double_value(option->value, 0.0,
			&config->cli.send_wait))
		return (set_value_error(config, option));
	else if (option->id == TRACEROUTE_OPT_WAIT_TIME
		&& parse_double_value(option->value, 0.0,
			&config->cli.wait_time))
		return (set_value_error(config, option));
	return (TRACEROUTE_ARGS_OK);
}

static int	apply_traceroute_options(t_traceroute_config *config,
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

static int	set_cli_error(t_traceroute_config *config,
		const t_arg_result *result) {
	config->cli.error = TRACEROUTE_ARGS_CLI;
	config->cli.cli_error = map_cli_error(result->error);
	config->cli.bad_arg = result->bad_arg;
	config->cli.bad_index = result->bad_index;
	return (config->cli.error);
}

static int	validate_traceroute_args(t_traceroute_config *config,
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
		return (set_range_error(config));
	config->cli.target = result->args[0];
	return (TRACEROUTE_ARGS_OK);
}

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
	error = arg_parser_parse(argc, argv, g_traceroute_specs, &result);
	if (error != ARG_OK)
	{
		set_cli_error(config, &result);
		arg_parser_result_clear(&result);
		return (config->cli.error);
	}
	error = apply_traceroute_options(config, &result);
	if (error == TRACEROUTE_ARGS_OK)
		error = validate_traceroute_args(config, &result);
	arg_parser_result_clear(&result);
	return (error);
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

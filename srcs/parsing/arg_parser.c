#include "arg_parser.h"
#include <stdlib.h>
#include <string.h>

static void	arg_parser_result_reset(t_arg_result *result) {
	result->option_count = 0;
	result->arg_count = 0;
	result->error = ARG_OK;
	result->bad_arg = NULL;
	result->bad_index = -1;
}

int	arg_parser_result_init(t_arg_result *result, int argc) {
	result->options = NULL;
	result->args = NULL;
	result->option_capacity = argc;
	result->arg_capacity = argc;
	arg_parser_result_reset(result);
	result->options = malloc(sizeof(*result->options) * argc);
	result->args = malloc(sizeof(*result->args) * argc);
	if (!result->options || !result->args)
	{
		arg_parser_result_clear(result);
		return (ARG_ERR_ALLOC);
	}
	return (ARG_OK);
}

void	arg_parser_result_clear(t_arg_result *result) {
	free(result->options);
	free(result->args);
	result->options = NULL;
	result->args = NULL;
	result->option_count = 0;
	result->arg_count = 0;
	result->option_capacity = 0;
	result->arg_capacity = 0;
	result->error = ARG_OK;
	result->bad_arg = NULL;
	result->bad_index = -1;
}

static const t_arg_spec	*find_short_spec(const t_arg_spec *specs, char c) {
	int	i;

	i = 0;
	while (specs[i].id != ARG_ID_END)
	{
		if (specs[i].short_name == c)
			return (&specs[i]);
		i++;
	}
	return (NULL);
}

static const t_arg_spec	*find_long_spec(const t_arg_spec *specs,
		const char *name, size_t length) {
	int	i;

	i = 0;
	while (specs[i].id != ARG_ID_END)
	{
		if (specs[i].long_name && strlen(specs[i].long_name) == length
			&& strncmp(specs[i].long_name, name, length) == 0)
			return (&specs[i]);
		i++;
	}
	return (NULL);
}

static int	add_option(t_arg_result *result, const t_arg_spec *spec,
		const char *raw, const char *value) {
	if (result->option_count >= result->option_capacity)
	{
		result->error = ARG_ERR_TOO_MANY_OPTIONS;
		result->bad_arg = raw;
		return (result->error);
	}
	result->options[result->option_count].id = spec->id;
	result->options[result->option_count].raw = raw;
	result->options[result->option_count].value = value;
	result->options[result->option_count].argv_index = result->bad_index;
	result->option_count++;
	return (ARG_OK);
}

static int	add_arg(t_arg_result *result, const char *arg, int argv_index) {
	if (result->arg_count >= result->arg_capacity)
	{
		result->error = ARG_ERR_TOO_MANY_ARGS;
		result->bad_arg = arg;
		result->bad_index = argv_index;
		return (result->error);
	}
	result->args[result->arg_count] = arg;
	result->arg_count++;
	(void)argv_index;
	return (ARG_OK);
}

static int	set_unknown_option(t_arg_result *result, const char *arg,
		int argv_index) {
	result->error = ARG_ERR_UNKNOWN_OPTION;
	result->bad_arg = arg;
	result->bad_index = argv_index;
	return (result->error);
}

static int	set_missing_value(t_arg_result *result, const char *arg,
		int argv_index) {
	result->error = ARG_ERR_MISSING_VALUE;
	result->bad_arg = arg;
	result->bad_index = argv_index;
	return (result->error);
}

static int	parse_short_option(const t_arg_spec *specs,
		t_arg_result *result, char **argv, int *i) {
	const t_arg_spec	*spec;
	const char			*value;

	spec = find_short_spec(specs, argv[*i][1]);
	if (!spec)
		return (set_unknown_option(result, argv[*i], *i));
	value = NULL;
	result->bad_index = *i;
	if (spec->has_value && argv[*i][2] != '\0')
		value = argv[*i] + 2;
	else if (spec->has_value && argv[*i + 1])
	{
		*i += 1;
		value = argv[*i];
	}
	else if (spec->has_value)
		return (set_missing_value(result, argv[*i], *i));
	else if (argv[*i][2] != '\0')
		return (set_unknown_option(result, argv[*i], *i));
	return (add_option(result, spec, argv[result->bad_index], value));
}

static int	parse_long_option(const t_arg_spec *specs,
		t_arg_result *result, char **argv, int *i) {
	const t_arg_spec	*spec;
	const char			*name;
	const char			*equal;
	const char			*value;
	size_t				length;

	name = argv[*i] + 2;
	equal = strchr(name, '=');
	length = strlen(name);
	if (equal)
		length = equal - name;
	spec = find_long_spec(specs, name, length);
	if (!spec)
		return (set_unknown_option(result, argv[*i], *i));
	value = NULL;
	result->bad_index = *i;
	if (spec->has_value && equal)
		value = equal + 1;
	else if (spec->has_value && argv[*i + 1])
	{
		*i += 1;
		value = argv[*i];
	}
	else if (spec->has_value)
		return (set_missing_value(result, argv[*i], *i));
	else if (equal)
		return (set_unknown_option(result, argv[*i], *i));
	return (add_option(result, spec, argv[result->bad_index], value));
}

int	arg_parser_parse(int argc, char **argv, const t_arg_spec *specs,
		t_arg_result *result) {
	int	i;
	int	end_options;

	i = 1;
	end_options = 0;
	arg_parser_result_reset(result);
	while (i < argc && result->error == ARG_OK)
	{
		if (end_options || argv[i][0] != '-' || argv[i][1] == '\0')
			add_arg(result, argv[i], i);
		else if (argv[i][1] == '-' && argv[i][2] == '\0')
			end_options = 1;
		else if (argv[i][1] == '-')
			parse_long_option(specs, result, argv, &i);
		else
			parse_short_option(specs, result, argv, &i);
		i++;
	}
	return (result->error);
}

const char	*arg_parser_error_name(int error) {
	if (error == ARG_OK)
		return ("ok");
	if (error == ARG_ERR_ALLOC)
		return ("allocation failed");
	if (error == ARG_ERR_UNKNOWN_OPTION)
		return ("unknown option");
	if (error == ARG_ERR_TOO_MANY_OPTIONS)
		return ("too many options");
	if (error == ARG_ERR_TOO_MANY_ARGS)
		return ("too many arguments");
	if (error == ARG_ERR_MISSING_VALUE)
		return ("missing option value");
	if (error == ARG_ERR_INVALID_VALUE)
		return ("invalid option value");
	return ("unknown parser error");
}

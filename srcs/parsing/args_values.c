
#include "args.h"

int	traceroute_parse_int_value(const char *str, int min, int max, int *out) {
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

static int	is_wait_separator(char c) {
	if (c == ',' || c == '/')
		return (1);
	return (0);
}

static int	parse_double_slice(const char *str, int start, int end,
		double *out) {
	char	buffer[64];
	int		i;

	if (end <= start || end - start >= (int)sizeof(buffer))
		return (-1);
	i = 0;
	while (start + i < end)
	{
		buffer[i] = str[start + i];
		i++;
	}
	buffer[i] = '\0';
	return (parse_double_value(buffer, 0.0, out));
}

static int	read_wait_value(const char *str, int *index, double *out) {
	int	start;

	start = *index;
	while (str[*index] && !is_wait_separator(str[*index]))
		(*index)++;
	return (parse_double_slice(str, start, *index, out));
}

static int	parse_wait_values(const char *str, double values[3], int *count) {
	int	i;

	i = 0;
	*count = 0;
	while (str[i] && *count < 3)
	{
		if (read_wait_value(str, &i, &values[*count]))
			return (-1);
		(*count)++;
		if (!str[i])
			return (0);
		if (!is_wait_separator(str[i]))
			return (-1);
		i++;
		if (!str[i])
			return (-1);
	}
	if (str[i])
		return (-1);
	return (0);
}

static int	apply_wait_values(t_traceroute_config *config,
		double values[3], int count) {
	if (values[0] <= 0.0)
		return (-1);
	config->cli.wait_time = values[0];
	if (count == 1)
	{
		config->cli.wait_here = 0.0;
		config->cli.wait_near = 0.0;
	}
	else if (count == 2)
	{
		config->cli.wait_here = values[1];
		config->cli.wait_near = TRACEROUTE_DEFAULT_WAIT_NEAR;
	}
	else if (count == 3)
	{
		config->cli.wait_here = values[1];
		config->cli.wait_near = values[2];
	}
	else
		return (-1);
	return (0);
}

int	traceroute_parse_wait_option(t_traceroute_config *config,
		const t_arg_option *option) {
	double	values[3];
	int		count;

	if (parse_wait_values(option->value, values, &count))
		return (traceroute_set_value_error(config, option));
	if (apply_wait_values(config, values, count))
		return (traceroute_set_value_error(config, option));
	return (TRACEROUTE_ARGS_OK);
}

int	traceroute_parse_send_wait_option(t_traceroute_config *config,
		const t_arg_option *option) {
	double	value;

	if (parse_double_value(option->value, 0.0, &value))
		return (traceroute_set_value_error(config, option));
	if (value > 10.0)
		value = value / 1000.0;
	config->cli.send_wait = value;
	return (TRACEROUTE_ARGS_OK);
}

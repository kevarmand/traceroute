#ifndef ARG_PARSER_H
# define ARG_PARSER_H

# include <stddef.h>

typedef enum e_arg_error {
	ARG_OK = 0,
	ARG_ERR_ALLOC,
	ARG_ERR_UNKNOWN_OPTION,
	ARG_ERR_TOO_MANY_OPTIONS,
	ARG_ERR_TOO_MANY_ARGS,
	ARG_ERR_MISSING_VALUE,
	ARG_ERR_INVALID_VALUE
}	t_arg_error;

typedef enum e_arg_id {
	ARG_ID_END = 0
}	t_arg_id;

typedef struct s_arg_spec {
	int			id;
	char		short_name;
	const char	*long_name;
	int			has_value;
}	t_arg_spec;

typedef struct s_arg_option {
	int			id;
	const char	*raw;
	const char	*value;
	int			argv_index;
}	t_arg_option;

typedef struct s_arg_result {
	t_arg_option	*options;
	const char		**args;
	int				option_count;
	int				arg_count;
	int				option_capacity;
	int				arg_capacity;
	int				error;
	const char		*bad_arg;
	int				bad_index;
}	t_arg_result;

int			arg_parser_result_init(t_arg_result *result, int argc);
void		arg_parser_result_clear(t_arg_result *result);
int			arg_parser_parse(int argc, char **argv, const t_arg_spec *specs,
				t_arg_result *result);
const char	*arg_parser_error_name(int error);

#endif

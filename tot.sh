#!/bin/bash

set -e

cat > srcs/output/error.c <<'EOF'
#include "ft_traceroute.h"
#include "arg_parser.h"
#include "args.h"

static void	print_cli_error(const t_traceroute_config *config) {
	printf("%s: %s", config->cli.program_name,
		arg_parser_error_name(config->cli.cli_error));
	if (config->cli.bad_arg)
		printf(": %s", config->cli.bad_arg);
	printf("\n");
}

static void	print_resolve_error(const t_traceroute_config *config) {
	if (config->target.error == TRACEROUTE_RESOLVE_GAI)
	{
		printf("%s: %s: %s\n", config->cli.program_name,
			config->cli.target, gai_strerror(config->target.gai_error));
		return;
	}
	printf("%s: %s: %s\n", config->cli.program_name, config->cli.target,
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
		printf("%s: %s\n", config->cli.program_name,
			traceroute_args_error_name(config->cli.error));
		return;
	}
	if (config->target.error != TRACEROUTE_RESOLVE_OK)
		print_resolve_error(config);
}
EOF

if ! grep -q "srcs/output/print_usage.c" Makefile; then
	sed -i '/srcs\/output\/print_header.c/a\		srcs/output/print_usage.c \\' Makefile
fi

rm -rf objs objs_debug

echo "done"
echo "now run: make re"
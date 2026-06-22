
#include "ft_traceroute.h"

static void	clear_addrinfo(struct addrinfo *hints) {
	unsigned char	*ptr;
	size_t			i;

	ptr = (unsigned char *)hints;
	i = 0;
	while (i < sizeof(*hints))
	{
		ptr[i] = 0;
		i++;
	}
}

static void	copy_string(char *dst, const char *src, size_t size) {
	size_t	i;

	i = 0;
	while (src[i] && i + 1 < size)
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
}

int	traceroute_resolve_target(t_traceroute_config *config) {
	struct addrinfo		hints;
	struct addrinfo		*result;
	struct sockaddr_in	*addr;
	int					error;

	clear_addrinfo(&hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	result = NULL;
	error = getaddrinfo(config->cli.target, NULL, &hints, &result);
	if (error != 0)
	{
		config->target.error = TRACEROUTE_RESOLVE_GAI;
		config->target.gai_error = error;
		return (config->target.error);
	}
	if (!result || result->ai_family != AF_INET)
	{
		if (result)
			freeaddrinfo(result);
		config->target.error = TRACEROUTE_RESOLVE_NO_INET;
		return (config->target.error);
	}
	addr = (struct sockaddr_in *)result->ai_addr;
	config->target.addr = *addr;
	config->target.addr_len = sizeof(config->target.addr);
	copy_string(config->target.ip, inet_ntoa(config->target.addr.sin_addr),
		sizeof(config->target.ip));
	freeaddrinfo(result);
	config->target.error = TRACEROUTE_RESOLVE_OK;
	return (TRACEROUTE_RESOLVE_OK);
}

const char	*traceroute_resolve_error_name(int error) {
	if (error == TRACEROUTE_RESOLVE_OK)
		return ("ok");
	if (error == TRACEROUTE_RESOLVE_GAI)
		return ("name resolution failed");
	if (error == TRACEROUTE_RESOLVE_NO_INET)
		return ("no IPv4 address found");
	return ("unknown resolution error");
}

int	traceroute_prepare_target(t_traceroute_config *config, int *exit_status) {
	int	error;

	error = traceroute_resolve_target(config);
	if (error != TRACEROUTE_RESOLVE_OK)
	{
		traceroute_print_error(config);
		*exit_status = 1;
		return (0);
	}
	return (1);
}

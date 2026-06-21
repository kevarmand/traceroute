#include "ft_traceroute.h"

static void	clear_bytes(void *ptr, size_t size) {
	unsigned char	*bytes;
	size_t			i;

	bytes = (unsigned char *)ptr;
	i = 0;
	while (i < size)
	{
		bytes[i] = 0;
		i++;
	}
}

static int	open_udp_socket(t_traceroute_config *config) {
	struct sockaddr_in	addr;
	int					fd;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0)
	{
		config->socket.error = errno;
		return (-1);
	}
	clear_bytes(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(0);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		config->socket.error = errno;
		close(fd);
		return (-1);
	}
	config->socket.udp_fd = fd;
	return (0);
}

static int	open_icmp_socket(t_traceroute_config *config) {
	int	fd;

	fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (fd < 0)
	{
		config->socket.error = errno;
		return (-1);
	}
	config->socket.icmp_fd = fd;
	return (0);
}

int	traceroute_open_sockets(t_traceroute_config *config) {
	config->socket.udp_fd = -1;
	config->socket.icmp_fd = -1;
	config->socket.error = 0;
	if (open_udp_socket(config) < 0)
		return (-1);
	if (open_icmp_socket(config) < 0)
	{
		close(config->socket.udp_fd);
		config->socket.udp_fd = -1;
		return (-1);
	}
	return (0);
}

int	traceroute_set_socket_ttl(t_traceroute_config *config, int ttl) {
	if (setsockopt(config->socket.udp_fd, IPPROTO_IP, IP_TTL,
			&ttl, sizeof(ttl)) < 0)
	{
		config->socket.error = errno;
		return (-1);
	}
	return (0);
}

void	traceroute_close_sockets(t_traceroute_config *config) {
	if (config->socket.udp_fd >= 0)
	{
		close(config->socket.udp_fd);
		config->socket.udp_fd = -1;
	}
	if (config->socket.icmp_fd >= 0)
	{
		close(config->socket.icmp_fd);
		config->socket.icmp_fd = -1;
	}
}
int	traceroute_prepare_sockets(t_traceroute_config *config,
		int *exit_status) {
	if (traceroute_open_sockets(config) < 0)
	{
		traceroute_print_error(config);
		*exit_status = 1;
		return (0);
	}
	return (1);
}

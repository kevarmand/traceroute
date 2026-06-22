

#include "session.h"

static t_probe	*find_printable_probe(t_session *session) {
	int			i;
	t_probe		*probe;

	i = 0;
	while (i < session->probe_capacity)
	{
		probe = &session->probes[i];
		if (probe->ttl == session->scheduler.print_ttl
			&& probe->probe_index == session->scheduler.print_probe_index
			&& (probe->state == PROBE_REPLY_PENDING
				|| probe->state == PROBE_TIMEOUT_PENDING))
			return (probe);
		i++;
	}
	return (NULL);
}

static void	start_hop_line(t_session *session) {
	if (!session->output.line_started)
	{
		printf("%2d ", session->scheduler.print_ttl);
		session->output.line_started = 1;
	}
}

static void	finish_hop_line(const t_traceroute_config *config,
		t_session *session) {
	if (session->scheduler.print_probe_index < config->cli.probes_per_hop)
		return;
	printf("\n");
	session->scheduler.print_probe_index = 0;
	session->scheduler.print_ttl++;
	session->output.line_started = 0;
	session->output.has_last_addr = 0;
}

static void	print_timeout_probe(t_probe *probe) {
	printf(" *");
	probe->state = PROBE_FREE;
}

static int	is_same_addr(const struct sockaddr_in *left,
		const struct sockaddr_in *right) {
	return (left->sin_addr.s_addr == right->sin_addr.s_addr);
}

static void	get_reply_ip(const t_probe *probe, char ip[INET_ADDRSTRLEN]) {
	if (!inet_ntop(AF_INET, &probe->reply_addr.sin_addr, ip, INET_ADDRSTRLEN))
		strcpy(ip, "0.0.0.0");
}

static void	print_reply_ip_only(const t_probe *probe) {
	char	ip[INET_ADDRSTRLEN];

	get_reply_ip(probe, ip);
	printf(" %s", ip);
}

static void	print_reply_dns_name(const t_probe *probe) {
	struct sockaddr_in	addr;
	char				host[NI_MAXHOST];
	char				ip[INET_ADDRSTRLEN];

	addr = probe->reply_addr;
	addr.sin_port = 0;
	get_reply_ip(probe, ip);
	if (getnameinfo((struct sockaddr *)&addr, sizeof(addr), host,
			sizeof(host), NULL, 0, NI_NAMEREQD) == 0)
		printf(" %s (%s)", host, ip);
	else
		printf(" %s (%s)", ip, ip);
}

static void	print_reply_addr(const t_traceroute_config *config,
		t_session *session, const t_probe *probe) {
	if (!session->output.has_last_addr
		|| !is_same_addr(&session->output.last_addr, &probe->reply_addr))
	{
		if (config->cli.no_dns)
			print_reply_ip_only(probe);
		else
			print_reply_dns_name(probe);
		session->output.last_addr = probe->reply_addr;
		session->output.has_last_addr = 1;
	}
}

static void	print_icmp_unreachable_suffix(const t_probe *probe) {
	if (probe->icmp_code == ICMP_NET_UNREACH)
		printf(" !N");
	else if (probe->icmp_code == ICMP_HOST_UNREACH)
		printf(" !H");
	else if (probe->icmp_code == ICMP_PROT_UNREACH)
		printf(" !P");
	else if (probe->icmp_code == ICMP_FRAG_NEEDED)
		printf(" !F");
	else if (probe->icmp_code == ICMP_SR_FAILED)
		printf(" !S");
	else if (probe->icmp_code == ICMP_NET_ANO
		|| probe->icmp_code == ICMP_HOST_ANO
		|| probe->icmp_code == ICMP_PKT_FILTERED)
		printf(" !X");
	else if (probe->icmp_code == ICMP_PREC_VIOLATION)
		printf(" !V");
	else if (probe->icmp_code == ICMP_PREC_CUTOFF)
		printf(" !C");
	else if (probe->icmp_code != ICMP_PORT_UNREACH)
		printf(" !%u", (unsigned int)probe->icmp_code);
}

static void	print_reply_suffix(const t_probe *probe) {
	if (probe->icmp_type == ICMP_DEST_UNREACH)
		print_icmp_unreachable_suffix(probe);
}

static void	print_reply_probe(t_traceroute_config *config,
		t_session *session, t_probe *probe) {
	print_reply_addr(config, session, probe);
	printf("  %.3f ms", probe->rtt_us / 1000.0);
	print_reply_suffix(probe);
	probe->state = PROBE_FREE;
}

static int	print_limit_reached(const t_session *session) {
	if (session->scheduler.stop_found
		&& session->scheduler.print_ttl > session->scheduler.stop_ttl)
		return (1);
	return (0);
}

void	traceroute_session_print_ready(t_traceroute_config *config,
		t_session *session) {
	t_probe	*probe;

	while (1)
	{
		if (print_limit_reached(session))
			return;
		probe = find_printable_probe(session);
		if (!probe)
			return;
		start_hop_line(session);
		if (probe->state == PROBE_TIMEOUT_PENDING)
			print_timeout_probe(probe);
		else if (probe->state == PROBE_REPLY_PENDING)
			print_reply_probe(config, session, probe);
		session->scheduler.print_probe_index++;
		finish_hop_line(config, session);
		fflush(stdout);
	}
}


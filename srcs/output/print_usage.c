#include "ft_traceroute.h"

void	traceroute_print_usage(const char *program_name) {
	(void)program_name;
	printf("Usage:\n");
	printf("  ft_traceroute [ -n ] [ -f first_ttl ] [ -m max_ttl ] ");
	printf("[ -N squeries ] [ -p port ] [ -q nqueries ] ");
	printf("[ -s src_addr ] [ -z sendwait ] [ -w waittime ] host\n");
	printf("Options:\n");
	printf("  -f first_ttl  --first=first_ttl\n");
	printf("                              Start from the first_ttl hop");
	printf(" (instead from 1)\n");
	printf("  -m max_ttl  --max-hops=max_ttl\n");
	printf("                              Set the max number of hops.");
	printf(" Default is 30\n");
	printf("  -N squeries  --sim-queries=squeries\n");
	printf("                              Set the number of probes to be tried\n");
	printf("                              simultaneously. Default is 16\n");
	printf("  -n                          Do not resolve IP addresses");
	printf(" to their domain names\n");
	printf("  -p port  --port=port        Set the destination port to use.");
	printf(" Default is 33434\n");
	printf("  -q nqueries  --queries=nqueries\n");
	printf("                              Set the number of probes per each hop.");
	printf(" Default is 3\n");
	printf("  -s src_addr  --source=src_addr\n");
	printf("                              Use source src_addr for outgoing packets\n");
	printf("  -z sendwait  --sendwait=sendwait\n");
	printf("                              Minimal time interval between probes.");
	printf(" Default is 0\n");
	printf("  -w waittime  --wait=waittime\n");
	printf("                              Time to wait for a response to a probe.");
	printf(" Default is 5\n");
	printf("  --help                      Read this help and exit\n");
	printf("\n");
	printf("Arguments:\n");
	printf("+     host          The host to traceroute to\n");
}


#ifndef SESSION_H
# define SESSION_H

# include "ft_traceroute.h"

# define TRACEROUTE_MAX_PROBE_SLOTS 256

typedef enum e_probe_state {
	PROBE_FREE = 0,
	PROBE_SENT,
	PROBE_REPLY_PENDING,
	PROBE_TIMEOUT_PENDING
}	t_probe_state;

typedef struct s_probe {
	t_probe_state		state;
	int					ttl;
	int					probe_index;
	unsigned short		port;
	struct timeval		sent_at;
	struct timeval		deadline;
	struct sockaddr_in	reply_addr;
	long				rtt_us;
	unsigned char		icmp_type;
	unsigned char		icmp_code;
}	t_probe;

typedef struct s_scheduler {
	int				send_ttl;
	int				send_probe_index;
	int				print_ttl;
	int				print_probe_index;
	unsigned short	next_port;
	struct timeval	next_send_at;
	int				in_flight_count;
	int				stop_found;
	int				stop_ttl;
}	t_scheduler;

typedef struct s_output_state {
	int					line_started;
	int					has_last_addr;
	struct sockaddr_in	last_addr;
}	t_output_state;

typedef struct s_session {
	t_scheduler		scheduler;
	t_output_state	output;
	t_probe			*probes;
	int				probe_capacity;
	int				total_probes;
}	t_session;

int		traceroute_prepare_session(t_traceroute_config *config,
			t_session *session, int *exit_status);
void	traceroute_cleanup_session(t_session *session);

int		traceroute_session_is_finished(const t_traceroute_config *config,
			const t_session *session);
int		traceroute_session_drain_replies(t_traceroute_config *config,
			t_session *session, int *exit_status);
void	traceroute_session_expire_probes(t_traceroute_config *config,
			t_session *session);
void	traceroute_session_print_ready(t_traceroute_config *config,
			t_session *session);
int		traceroute_session_send_ready(t_traceroute_config *config,
			t_session *session, int *exit_status);
int		traceroute_session_wait(t_traceroute_config *config,
			t_session *session, int *exit_status);

#endif


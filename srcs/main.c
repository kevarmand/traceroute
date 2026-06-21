#include "ft_traceroute.h"
#include "session.h"
#include "debug/debug.h"

int	main(int argc, char **argv) {
	t_traceroute_config	config;
	t_session			session;
	int					exit_status;

	exit_status = 0;

	//parsing des arguments
	if (!traceroute_prepare_cli(argc, argv, &config, &exit_status))
		return (exit_status);
	DEBUG_ARGS_PARSE(&config);

	//preparation de la cible
	if (!traceroute_prepare_target(&config, &exit_status))
		return (exit_status);
	DEBUG_RESOLVE(&config);

	//ouverture des sockets
	if (!traceroute_prepare_sockets(&config, &exit_status))
		return (exit_status);
	DEBUG_SOCKET(&config);

	//initialisation des structures runtime
	if (!traceroute_prepare_session(&config, &session, &exit_status))
	{
		traceroute_close_sockets(&config);
		return (exit_status);
	}

	traceroute_print_header(&config);

	//boucle principale
	while (!traceroute_session_is_finished(&config, &session))
	{
		//lire les reponses ICMP deja disponibles
		if (!traceroute_session_drain_replies(&config, &session, &exit_status))
			break;

		//marquer les probes expirees
		traceroute_session_expire_probes(&config, &session);

		//afficher les probes pretes pour le hop courant
		traceroute_session_print_ready(&config, &session);

		//envoyer les probes autorisees par le scheduler
		if (!traceroute_session_send_ready(&config, &session, &exit_status))
			break;

		//attendre le prochain evenement utile
		if (!traceroute_session_wait(&config, &session, &exit_status))
			break;
	}

	//free toutes la memoire et close les sockets
	traceroute_cleanup_session(&session);
	traceroute_close_sockets(&config);
	return (exit_status);
}

#include "arg_client.h"

static void
usage(const char *name)
{
    fprintf(stdout, "Usage: %s [OPTIONS]\n", name);
    fputs("Messages Client\n", stdout);
    fputs("\n", stdout);
    fputs("-s   ip:port     define src client ip and port (default: localhost:8283) - UDP\n", stdout);
    fputs("                 E.g. 127.0.0.1:2222, 192.168.1.2\n", stdout);
    fputs("                      port can be set only if you set an ip first\n", stdout);
    fputs("-d   ip:port     define dst server ip and port (default: localhost:8282) - TCP\n", stdout);
    fputs("                 E.g. 127.0.0.1:2222, 192.168.1.2\n", stdout);
    fputs("                      port can be set only if you set an ip first\n", stdout);
    fputs("-h               print this help and exit\n", stdout);
    fputs("-v               run in verbose mode, for debug purpose\n", stdout);
}

static void 
set_ip_port(char **addr,int *port){
	char 	*token;
	char 	*rest;
	char    *ip;
	struct 	sockaddr_in sa;
	bool 	is_ip_setted;


	token = strtok_r(optarg, ":", &rest);
	ip = token;
	strcmp("localhost", token) == 0  ? (ip = "127.0.0.1") : (ip = token);
	if (token != NULL &&  inet_pton(AF_INET, ip, &sa.sin_addr) == 1)
	{	
		*addr = malloc(strlen(ip)*sizeof(char));
		stpcpy(*addr, ip);
		is_ip_setted = true;
	}
	else{
		fprintf(stdout, "Oops!\nPls insert a valid ip and a valid port\n\n");
		exit(EXIT_FAILURE);
	}

	if ( rest != NULL && is_valid_port(atoi(rest))){
		*port =  atoi(rest);
	}
	else if( !is_ip_setted ){
		fprintf(stdout,"Oops!\nPls insert a valid ip and a valid port\n\n");
		exit(EXIT_FAILURE);
	}
}

void
init_argv(struct client_entity* client, struct server_entity *server,const int argc, char  **argv)
{
	int 	c; 					// temp arg for getopt, GNU c lib std
	//char 	cmd_value[BUFFER_SIZE];


	while ((c = getopt(argc, argv, "s:d:hv")) != -1){
		switch (c){
			case 's':
				set_ip_port(&client->addr, &client->udp_port);
				break;
			case 'd':
				set_ip_port(&server->addr, &server->tcp_port);
				break;
			case 'v':
				log_set_quiet(QUIET_OFF);
				log_set_level(LOG_INFO);
				break;
			case 'h':
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
				if (optopt == 's' || optopt == 'd'){
					fprintf ( stderr, 
						      "Option -%c requires an argument.\n", optopt);
					 usage(argv[0]);
				}
				else if (isprint (optopt)){
				  	fprintf ( stderr, 
				  			  "Unknown option `-%c'.\n", optopt);
					 usage(argv[0]);
				}
				else{
					//char is not printable
				  	fprintf (stderr,
				    	     "Unknown option character `\\x%x'.\n",
				             optopt);
					usage(argv[0]);
				}
				exit(EXIT_FAILURE);
			default:
				abort ();
		}
	}

}
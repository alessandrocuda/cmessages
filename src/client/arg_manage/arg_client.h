#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../../log/log.h"
#include "../../utils/util_net.h"

#define BUFFER_SIZE 1024


void 
init_argv( 	struct client_entity* client, struct server_entity *server, 
			const int argc, char  **argv);


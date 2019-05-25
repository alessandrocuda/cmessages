#ifndef CONTROLLER_SERVER_H
#define CONTROLLER_SERVER_H

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "../log/log.h"
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include "../utils/util_net.h"
#include "../utils/model_data.h"


/*
 * status code of controller server
 */
enum{
	ALL_OK		= 701,      // command ended successfully
	ERROR_EXC 	= 702       // net and protocol error
};

#define BUFFER_SIZE 	1024

char* 
read_command();

void 
set_controller_data(int sock, struct reg_client** client_head);

int 
register_command();

int 
who_command();

int 
quit_command();

int 
deregister_command();

int 
send_command();

#endif

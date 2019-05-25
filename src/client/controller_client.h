#ifndef CONTROLLER_CLIENT_H
#define CONTROLLER_CLIENT_H 


#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <regex.h>  
#include <pthread.h>
#include "../utils/model_data.h"
#include "../utils/util_net.h"
#include "../log/log.h"


#define 	BUFFER_SIZE		1024

/*
 * 	Status codes of controller client
 */
enum controller_status
{	
	RESULT_SETTED 			= 301,		// controller setted a result, you can get it with get_result_controller()
	ERROR_CONNECT_TO_SERVER = 302,		// not possible to connect to the server
	ALREADY_REG				= 303,		// you are trying to regist but you arleady did it
	REG_SUCCEEDED			= 304,		// registration finished successfully
	REG_USER_WITH_SAME_NAME = 305,		// client try to regist but a user is registerd with the same username
	NOT_REGISTERD 			= 306,      // you are trying to do a command but you have to be registered for it
	USER_WHERE_OFFLINE		= 307,		// an user with that username was offline, user will go back online
	CANNOT_SEND_MSG_TO_YOU	= 308,		// try to send a messange but client dst cannot be yourself
	CLIENT_NOT_FOUND		= 309,		// try to send a message to a user, user not found
	ALL_OK 					= 310,		// command ended successfully
	CLIENT_GO_OFFLINE 		= 311,		// client ask to close, user will go offline if registered
	DEREG_SUCCEEDED			= 312,		// deregistration ended successfully
	ERROR_EXC 				= 313 		// net and protocol error
};


void 
init_controller(char *cmd, char *cmd_context); 

int 
who_command(struct server_entity server);

int 
set_offline_command(struct server_entity server, struct client_entity client);

int
deregister_command(struct server_entity server, struct client_entity *client);

int
register_command(struct server_entity server, struct client_entity *client);

int
send_command(struct server_entity server, struct client_entity *client);

char*
get_result_controller();

void*
live_msgs(void *client);

#endif

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include "log/log.h"
#include <regex.h>  
#include <pthread.h>


/*
 * 	Regex used to valid username
 *  min length: 3 
 *  max	length: 15
 * 	only alphanumeric characters, case sensitive
 */
#define SERVER_VERSION 1.0.0

#define REGEX_USERNAME "^[a-zA-Z0-9_-]{3,15}$"

#define BUFFER_SIZE 1024

#define HELP "The following commands are available:\n"									\
			 "   !help                  show commands list\n" 							\
			 "   !register username     registers the client at the server\n" 			\
			 "                          'username' must be min 3 to max 15,\n"			\
			 "                          only alphanumeric chars, - / _ and no space\n"	\
			 "   !deregister            de-register client from server\n" 				\
			 "                          (you must be online)\n"							\
			 "   !who                   show clients list available\n"					\
			 "   !send username         send a message to another client\n" 			\
			 "   !quit                  set client offline and exit!\n\n"		
	

#define CLIENT_DEFAULT()            \
{                                   \
	.name		= NULL,				\
    .addr       = "127.0.0.1",      \
    .udp_port   = 8283,             \
    .is_reg		= REG_NO,			\
}

#define SERVER_DEFAULT()            \
{                                   \
    .addr       = "127.0.0.1",      \
    .tcp_port   = 8282,             \
}



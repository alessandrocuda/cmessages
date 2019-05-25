#ifndef UTIL_NET_H
#define UTIL_NET_H 

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include "model_data.h"
#include "../log/log.h"


#define REG_YES 1
#define REG_NO 	0

#define STS_ONLINE 1
#define STS_OFFLINE 0

/*
 * 	Status enum
 */
enum {  REG_OK 			= 400,
		REG_USR_ONLINE	= 401,
		REG_USR_OFFLINE = 402,
		CLIENT_NOT_EXIT = 403,
		CLIENT_ONLINE	= 404,
		CLIENT_OFFLINE	= 405,
		MSG_IN_QUEUEE	= 406,
		NO_MSG_QUEUEE	= 407
};


int
create_server(struct sockaddr_in *sockaddr,
			  const int port);

int
connect_to_server(struct sockaddr_in *sockaddr, 
					struct server_entity server );
int
send_string(int sd, char *str);

char*
rcv_string(int sd);

int
create_udp_socket(struct sockaddr_in *sockaddr, char *ip, int port);

void
send_instant_msg(char* msgs, char *ip, int port);

bool
is_valid_port(int port);
#endif
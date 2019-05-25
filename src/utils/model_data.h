#ifndef UTIL_LINKLIST_H
#define UTIL_LINKLIST_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../log/log.h"

#define SPC_DIM 	1
#define NL_DIM 		1
#define IP_LENGTH 	17

/*
 * every client registered on the server 
 * could have a message queue
 */
struct msg_entity
{
	char* 			 	msg;
	struct msg_entity 	*next;
};

/*
 * Client entity 
 * structure that contains 
 * all the information about a client
 */
struct client_entity
{	
	char* 	name;					// name of the client
	char*	addr;					// ip address reachable by other clients
	int   	udp_port;				// port used by the client to rcv instat message
	bool 	is_reg; 				// if client is registered on the server 
};

/*
 * Server entity 
 * structure that contains 
 * all the information about a client
 */
struct server_entity
{	
	char*	addr;   				// ip used to contact the server
	int   	tcp_port;				// server port 
};

/*
 * 	Linked List, manage registered clients on the server
 */
struct reg_client
{	
	char*	name;					// name of the client
	char*	addr;					// ip address reachable by other clients
	int   	udp_port;				// port used by the client to rcv instat message
	bool	status;					// client online or offline
	struct 	msg_entity *queuee_msg; // message queue, store messages when a client is offline
	struct 	reg_client *next;		// pointer to the next registered client

};


struct reg_client* 
add_client(struct reg_client* head, struct reg_client* data);

char* 
get_all_clients(struct reg_client* head);

void 
init_reg_client(struct reg_client** head);

struct reg_client*
find_client(struct reg_client* head, char *name);

bool 
delete_client(struct reg_client** head, char *name);

void
free_mem_client(struct reg_client* head);

struct msg_entity*
add_msg_to_client(struct msg_entity* head, struct msg_entity* data);

char*
get_queuee_msgs(struct msg_entity** head);

void
free_mem_msgs(struct msg_entity* head);


#endif

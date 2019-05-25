/*
 * Controller client, manage all operation that client want to do.
 * as part of the Model - view - controller pattern,
 * this is the middle part use model, manage information and alert 
 * the to the view what to do 
 * Operation:
 *  -   get all registered clients
 *  -   set offline a online user (user must be already online)
 *  -   deregister a user on the server
 *  -   register a user on the server
 *  -   send a message to a client offline/online
 *	- 	client (view code) can retrieve result from the controller as replay from the server
 *  -   UDP server, for retrive live messsages from other client
 */
#include "controller_client.h"

char		*command;								// contain command
char 		*cmd_par;								// command par (E.g. !register ale)
struct 		sockaddr_in 	sv_addr;				// server info
char		*result;								// result to retrive by controler 



/* 
 * ----------------------------------------------------------------- 
 *|               Read message to send from keyboard 
 * ----------------------------------------------------------------- 
 * \param   *msg    header message
 * \return 	msgs 	return message to send
 */
static char*
get_message(char *msg){
	char 		*msgs;
	int 		len;

	len = strlen(msg)+1;
	msgs = (char *) malloc(sizeof(char)*len);
	strcpy(msgs, msg);
	while(strcmp(msg, ".\n")!=0){
		fgets(msg, BUFFER_SIZE, stdin);
		len = len + strlen(msg);
		msgs = (char*) realloc(msgs, sizeof(char)*(len+1));
		strcat(msgs, msg);
	}
	len = strlen(msgs);
	if (msgs[len-2] == '.')
	{
		msgs[len-2]='\0';	
	}
	return msgs;
}

/* 
 * ----------------------------------------------------------------- 
 *|  Retrieve result from the controller as replay from the server
 * ----------------------------------------------------------------- 
 * \return 	result 	return result from the controller
 *
 *	E.g. !who command 	-> 	result is the list of users
 * 	E.g. !register ale 	-> 	ale where offline, 
 *						    result will contain offline msgs
 */
char*
get_result_controller(){
	return result;
}


/* 
 * ----------------------------------------------------------------- 
 *|    Init controller, set command and parameter command
 * ----------------------------------------------------------------- 
 * \desc: 	View (client) can use this to set up which command
 *			and parameter controller have to use
 * \param:	*cmd 			specify the command
 * \param:	*cmd_context 	specify the command argument
 */
void 
init_controller(char *cmd, char *cmd_context){
	command = cmd;
	cmd_par = cmd_context;
}



/* 
 * ----------------------------------------------------------------- 
 *|   	 !who command, get all registered clients
 * ----------------------------------------------------------------- 
 * \desc:	connect to server, save the result from the server
 *			on the global var 'result'
 * \param:	server 			specify which server to contact
 * \return:	int  	 		Status Code, for more details look
 *							in the controller_client.h file
 */
int 
who_command(struct server_entity server){
	int sd;
	sd = connect_to_server(&sv_addr, server);
	if (sd < 0)
		return ERROR_CONNECT_TO_SERVER;
	if (send_string(sd,command) < 0) goto error;
	result = rcv_string(sd);
	if(result == NULL) goto error;
	close(sd);
	return RESULT_SETTED;

	error:
		close(sd);
		return ERROR_EXC; 
}


/*
 * ----------------------------------------------------------------- 
 *    set offline a online user (user must be already online)
 * -----------------------------------------------------------------
 * \desc:	connect to server, set offline user
 * \param:	server 			specify which server to contact
 * \param:  client          specify which client have to go offline
 * \return:	int  	 		Status Code, for more details look
 *							in the controller_client.h file
 */
int 
set_offline_command(struct server_entity server, struct client_entity client)
{	
	log_info("\nStart set_offline_command:");
	int sd;
	if(!client.is_reg)
		return NOT_REGISTERD;
	sd = connect_to_server(&sv_addr, server);
	if (sd < 0)
		return ERROR_CONNECT_TO_SERVER;
	log_info("Send command: %s and username: %s",command,client.name);
	if ( send_string(sd,command) < 0 ) goto error;
	if ( send_string(sd, client.name) < 0 ) goto error;
	close(sd);
	return CLIENT_GO_OFFLINE;

	error:
		close(sd);
		return ERROR_EXC;
}


/*
 * -----------------------------------------------------------------
 *              deregister a user on the server
 * -----------------------------------------------------------------
 * \desc:    deregister a user on the server 
 * \param:   server          specify which server to contact
 * \param:   client          specify which client to deregister
 * \return:  int             Status Code, for more details look
 *                           in the controller_client.h file
 */
int
deregister_command(struct server_entity server, struct client_entity *client)
{	
	int sd;
	if (!client->is_reg)
		return NOT_REGISTERD;
	sd = connect_to_server(&sv_addr, server);
	if (sd < 0)
		return ERROR_CONNECT_TO_SERVER;
	if ( send_string(sd,command) < 0 ) goto error;
	if ( send_string(sd, client->name) < 0 ) goto error;
	client->name = NULL;
	client->is_reg = REG_NO;
	close(sd);
	return DEREG_SUCCEEDED;

	error:
		close(sd);
		return ERROR_EXC; 

}


/*
 * -----------------------------------------------------------------
 *              register a user on the server
 * -----------------------------------------------------------------
 * \desc:    register a user, there are three cases:
 *           1: unregistered user, then register it
 *           2: name already used, can't register
 *           3: user offline, get it back online
 *
 * \param:   server          specify which server to contact
 * \param:   client          specify which client register
 * \return:  int             Status Code, for more details look
 *                           in the controller_client.h file
 */
int
register_command(struct server_entity server, struct client_entity *client){
    char	client_string[BUFFER_SIZE]; 				// buffer for serialization
    int 	sd;
    int 	status = ERROR_EXC;
    char 	*ret_rcv;
	
    if( client->is_reg )
		return ALREADY_REG;

	sd = connect_to_server(&sv_addr, server);			// 1 - Connect to Server
	if (sd < 0)
		return ERROR_CONNECT_TO_SERVER;
	if ( send_string(sd,command) < 0 ) goto close;		// 2 - Send command to Server				 
	if ( send_string(sd,cmd_par) < 0 ) goto close;		// 3 - Send Name client to Server
	
	ret_rcv = rcv_string(sd);                           // wait response from the server
	if(ret_rcv == NULL) goto close;

	switch(atoi(ret_rcv)){
		case REG_OK:                                    // 4.1 - can register, send data to the server
			free(ret_rcv);
			sprintf(client_string,"%s %d",client->addr, client->udp_port);
			if (send_string(sd, client_string) < 0 ) goto close;    //send data
			ret_rcv = rcv_string(sd);                   // rcv status
			if(ret_rcv == NULL) goto close;
			if ( atoi(ret_rcv) == REG_OK )              // wait ACK and then save the data on the client too
			{	
				free(ret_rcv);
				int len;
				len = strlen(cmd_par);
				client->name = (char * ) malloc(sizeof(char)*len);
				strcpy(client->name,cmd_par);
				client->is_reg = REG_YES;
				status = REG_SUCCEEDED;
			}
			break;
		case REG_USR_ONLINE:                            // 4.2 - can't register, user with the same name online
			free(ret_rcv);
			status = REG_USER_WITH_SAME_NAME;
			break;
		case REG_USR_OFFLINE:                           // 4.3 - back online
			free(ret_rcv);                              // send ip and port, could be changed from the last time
			sprintf(client_string,"%s %d",client->addr, client->udp_port);
			if ( send_string(sd, client_string) < 0 ) goto close;

			ret_rcv = rcv_string(sd);                   // 4.3.1 rcv queuee status
			if(ret_rcv == NULL) goto close;

			if (atoi(ret_rcv) == MSG_IN_QUEUEE){        // 4.3.2 there is mgs in the queuee
				free(ret_rcv);
				result = rcv_string(sd);
				if(result == NULL) goto close;
			}
			else
				result = NULL;
			client->name = (char * ) malloc(strlen(cmd_par)*sizeof(char));
			strcpy(client->name,cmd_par);
			client->is_reg = REG_YES;
			status = USER_WHERE_OFFLINE;
			break;
	}
	close:
		close(sd);
		return status;
}


/*
 * -----------------------------------------------------------------
 *         Send a message to a client offline/online
 * -----------------------------------------------------------------
 * \desc:    send a msgs to a recipent client
 * \param:   server          specify which server to contact
 * \param:   client          specify who want to send the msg
 * \return:  int             Status Code, for more details look
 *                           in the controller_client.h file
 */
int
send_command(struct server_entity server, struct client_entity *client){
    struct 		client_entity 	client_dst;   // struct recipient client

	char 		msg[BUFFER_SIZE];
	char 		*msgs;
	char 		*data;
	int 		sd;
	int 		status = ERROR_EXC;
	char 		*ret_rcv;


	if(!client->is_reg)                             // cannot send msg if the sender is not register
		return NOT_REGISTERD;

	if (strcmp(client->name, cmd_par) == 0)         // cannote send msg to the sender
		return CANNOT_SEND_MSG_TO_YOU;

	msg[0] = '\0';	    	
	sd = connect_to_server(&sv_addr, server);       // connect to the server
	if (sd < 0)
		return ERROR_CONNECT_TO_SERVER;
	
	if ( send_string(sd, command) < 0 ) goto close; // send the command
	if ( send_string(sd, cmd_par) < 0 ) goto close; // send username to contact

    ret_rcv = rcv_string(sd);                       // rcv a status code:
	if ( ret_rcv == NULL) goto close;

	switch(atoi(ret_rcv)){
		case CLIENT_NOT_EXIT:                       // recipient client not exist on the server
			free(ret_rcv);
			status = CLIENT_NOT_FOUND;
			break;
		case CLIENT_ONLINE:                         // recipient client is online
			free(ret_rcv);
			data = rcv_string(sd);                  // get addr and udp_port of recipient client
			if ( data == NULL) goto close;
			client_dst.name = (char*) malloc(strlen(cmd_par)*sizeof(char));
			strcpy(client_dst.name, cmd_par);
			fprintf(stdout, " - %s is online, send a message!\n",client_dst.name);
			client_dst.is_reg = REG_YES;

			client_dst.addr = (char*) malloc(IP_LENGTH);
	    	sscanf(data,"%s %d", client_dst.addr, &client_dst.udp_port);

	    	log_info(" - user to contact info:\n - name: %s\n - ip: %s\n - udp_port: %d",
	    							client_dst.name,client_dst.addr,client_dst.udp_port);
			
			sprintf(msg,"%s (Instat Msg)>\n",client->name);
			msgs = get_message(msg);                // read message from keyboard and send instat msg
			send_instant_msg(msgs, client_dst.addr, client_dst.udp_port);
			fputs("Instant Messange has been sent\n", stdout);
	    	free(client_dst.name);
	    	free(data);
	    	free(msgs);
	    	status = ALL_OK;
			break;
		case CLIENT_OFFLINE:                        // recipient client offline, send msg to recipient client queuee msgs
			free(ret_rcv);
			fprintf(stdout, "user: %s is offline, send messages in offline mode:\n",cmd_par);
			sprintf(msg,"%s (msg offline)>\n",client->name);
			msgs = get_message(msg);
			if ( send_string(sd, msgs) < 0 ){
				free(msgs);
				goto close;
			} 
			free(msgs);
			status = ALL_OK;
			break;
	}
	
	close:
		fflush(stdout);
		close(sd);
		return status;
}

/*
 * -----------------------------------------------------------------
 *   UDP server, for retrive live messsages from other client
 * -----------------------------------------------------------------
 * \desc:    pointer fun, use it to implement a thread udp server
 * \param:   client          retrive client ip and udp for set up
 *                           the udp server
 */
void 
*live_msgs(void *client)
{	
	struct 		sockaddr_in 	my_addr, client_addr;
	struct 		client_entity 	*my_client;
	char 		buff[BUFFER_SIZE];
	int 		sd, len;

	my_client = (struct client_entity*) client;
	log_info("\nStart thread - live msgs");
	log_info("listen at %s:%d",my_client->addr,my_client->udp_port);
	sd = create_udp_socket(&my_addr, my_client->addr, my_client->udp_port);
	while(1){
		recvfrom(sd, buff, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr,(socklen_t*) &len);
		fprintf(stdout,"\n%s\n%s >",buff, my_client->name );
		fflush(stdout);
	}
	close(sd);
	pthread_exit(NULL);	
}

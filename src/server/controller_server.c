/*
 * Controller server, manage all operation that server want to do.
 * as part of the Model - view - controller pattern,
 * this is the middle part use model, manage information and alert
 * the server on all operations managed by the server
 * Operation:
 * - Read a command from a Client
 * - Manage a registration by a user
 * - Get list of registered user and send it to a client user
 * - Manage the disconnection of a user and making it in offline status
 * - Manage the deregistration of a user
 * - manage message sending from one user to another
 */
#include "controller_server.h"

int 			sock_connect;               // file descripot of a inited socket
                                            // the socket currently initialized by an accept

struct 			reg_client *client_head;	// global data model beetween threads
                                            // pointer shared beetween threads
                                            // functions use this pointer in mutual exclusion

// mutex shared between threads
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


/*
 * -----------------------------------------------------------------
 *|               Set the controller server data
 * -----------------------------------------------------------------
 * \param   sock        socket initialized by an accept,
 *                      used to init sock_connect
 * \param   **clients   pointer of a reg_client struct,
 *                      used to init cient_head
 */
void 
set_controller_data(int sock, struct reg_client** clients){
	sock_connect = sock;
	client_head  = *clients;
}


/*
 * -----------------------------------------------------------------
 *|                Read a command from a Client
 * -----------------------------------------------------------------
 * \return   *cmd_buff  return string of command sent by user
 */
char * 
read_command(){
	char *cmd_buff;
	cmd_buff = rcv_string(sock_connect); //read command
	if ( cmd_buff == NULL){
		return NULL;
	}
	log_debug("command: %s",cmd_buff);
	return cmd_buff;
}


/*
 * -----------------------------------------------------------------
 *|                Manage a registration by a user
 * -----------------------------------------------------------------
 * \desc:    register a user, there are three cases:
 *           1: unregistered user, then register it
 *           2: name already used, can't register
 *           3: user offline, get it back online
 *
 * \return:  int    Status Code, for more details look
 *                  in the controller_server.h file
 */
int 
register_command(){
  	char    *buffer;
	struct 	reg_client *data_client;
	struct  reg_client *tmp_data_client;
	char	sts_code[BUFFER_SIZE];

	log_info(" - Start registration procedure");

    buffer = rcv_string(sock_connect);                                      // rcv arg of !register
    if ( buffer == NULL ) 
    	return ERROR_EXC;
	data_client = (struct reg_client *) malloc(sizeof(struct reg_client));
    data_client->name = buffer;

    pthread_mutex_lock(&mutex);                                             // lock for data safety
	log_debug(" - User %s want to register", data_client->name);
	tmp_data_client = find_client(client_head, data_client->name);          // check if an user with that name
                                                                            // is already registered

	if (tmp_data_client != NULL)                                            // username already registered
	{	
		free(data_client);
		log_info(" - user: %s try to register but the same username is already taken",
                                                                tmp_data_client->name);
		if(tmp_data_client->status)                                         // this user is online
		{
			log_info(" - there is a user online with the same name",
                                                     tmp_data_client->name);
			sprintf(sts_code,"%d",REG_USR_ONLINE);                          // send status code
			if ( send_string(sock_connect, sts_code) < 0 ) return ERROR_EXC;
   		}else{
			sprintf(sts_code,"%d",REG_USR_OFFLINE);                         // this user is online
			if ( send_string(sock_connect, sts_code) <0 ) return ERROR_EXC;
    		buffer = rcv_string(sock_connect);                              // rcv update value of ip/udp_port
    		if ( buffer == NULL) return ERROR_EXC;
	    	sscanf(buffer,"%s %d",
                          tmp_data_client->addr, &tmp_data_client->udp_port);
			tmp_data_client->status=STS_ONLINE;
			log_info(" - user: %s was offline, get back it online",
                                                      tmp_data_client->name);
			log_info(" - update ip:port user: %s:%d (could be changed)", 
							tmp_data_client->addr,tmp_data_client->udp_port);
                                                                            // check if there is a queuee msgs
			log_debug("Check if there is message in pending to use: %s",
                                                            data_client->name);
			if (tmp_data_client->queuee_msg != NULL)                        // yes, send it
			{
				char *msg_queuee;
				sprintf(sts_code,"%d",MSG_IN_QUEUEE);                       // send the sts code
				if ( send_string(sock_connect, sts_code) <0 )return ERROR_EXC;
				log_info(" - send message in queuee to user: %s",
                                                    tmp_data_client->name);
				msg_queuee = get_queuee_msgs(&tmp_data_client->queuee_msg);
				if ( send_string(sock_connect, msg_queuee) < 0)
				{
					free(msg_queuee);
					return ERROR_EXC;
				};
				free(msg_queuee);
			}else{
				sprintf(sts_code,"%d",NO_MSG_QUEUEE);                       // no, send sts code
			}
   		}
	}else{                                                                  // client can register with that username
		log_info(" - wait user information");
		sprintf(sts_code,"%d",REG_OK);
		if ( send_string(sock_connect, sts_code) < 0 )                      // alert client, it can register
			return ERROR_EXC;
    	buffer = rcv_string(sock_connect);                                  // wait for client data
    	if ( buffer == NULL) return 
    		ERROR_EXC;
    	sprintf(sts_code,"%d",REG_OK);                                      // alert client, done
		if ( send_string(sock_connect, sts_code) < 0) 
			return ERROR_EXC;
    	
    	/*
	     * add client to the main list
	     */
    	data_client->addr = (char*) malloc(IP_LENGTH);
    	sscanf(buffer,"%s %d", data_client->addr, &data_client->udp_port);
    	data_client->status = STS_ONLINE;
    	data_client->queuee_msg =NULL;
    	data_client->next = NULL;
    	client_head = add_client(client_head, data_client);

    	log_info(" - user: %s has been registered ",data_client->name);
		log_info(" - with this information; ");
		log_info(" - ip: %s ",data_client->addr);
		log_info(" - udp_port: %d ",data_client->udp_port);
	}
	pthread_mutex_unlock(&mutex);                                           // unlock data
	return ALL_OK;
}


/*
 * -----------------------------------------------------------------
 *|   Get list of registered user and send it to a client user
 * -----------------------------------------------------------------
 * \return:    int               Status Code, for more details look
 *                               in the controller_client.h file
 */
int 
who_command(){
	char 	*client_info;
	    	
	log_info(" - Start !who request");
	pthread_mutex_lock(&mutex);
	client_info = get_all_clients(client_head);
	pthread_mutex_unlock(&mutex);
	if ( send_string(sock_connect, client_info) < 0 ) return ERROR_EXC;
	free(client_info);
	return ALL_OK;
}


/*
 * -----------------------------------------------------------------
 *|  Manage the disconnection user and making it in offline status
 * -----------------------------------------------------------------
 * \return:    int               Status Code, for more details look
 *                               in the controller_client.h file
 */
int 
quit_command(){
	char 	*client_name;
	struct  reg_client 	*tmp_data_client;
	int 	status = ERROR_EXC;

	log_info(" - Start !quit request");
	pthread_mutex_lock(&mutex);

	client_name = rcv_string(sock_connect);                                  // rcv client name
	if ( client_name == NULL) goto close;
	tmp_data_client = find_client(client_head, client_name);                 // check if username is registered
	if (tmp_data_client != NULL)
	{
		tmp_data_client->status = STS_OFFLINE;                               // set offline
		log_info(" - User: %s setted %s",
			tmp_data_client->name,tmp_data_client->status ? "online" : "offline");
	}
	free(client_name);
	status = ALL_OK;                                                         // send ack to client
	
	close:
		pthread_mutex_unlock(&mutex);
		return status;
}


/*
 * -----------------------------------------------------------------
 *|             Manage the deregistration of a user
 * -----------------------------------------------------------------
 * \return:    int               Status Code, for more details look
 *                               in the controller_client.h file
 */
int 
deregister_command(){
	char	*client_name;

	log_info(" - Start !deregister request");
	client_name = rcv_string(sock_connect);
	if ( client_name == NULL)
		return ERROR_EXC;
	log_info(" - %s want to deregister",client_name);
	pthread_mutex_lock(&mutex);
	delete_client(&client_head, client_name);
    pthread_mutex_unlock(&mutex);
	log_info(" - %s has been deregister",client_name);
	free(client_name);
	return ALL_OK;
}


/*
 * -----------------------------------------------------------------
 *|       manage message sending from one user to another
 * -----------------------------------------------------------------
 * \return:    int               Status Code, for more details look
 *                               in the controller_client.h file
 */
int 
send_command(){
 	char    *buffer;
	char 	*msg;
 	struct 	reg_client *data_client;
	char 	sts_code[BUFFER_SIZE];
	char 	client_string[BUFFER_SIZE];
	struct 	msg_entity *msg_t;

    
	log_info(" - Start !send request");
	buffer = rcv_string(sock_connect);                                  // arg of !send
	if (buffer == NULL)
		return ERROR_EXC;
	log_info("  - An User want to send a msg to user: %s",buffer);
	data_client = find_client(client_head, buffer);                     // find the user to contact
	if (data_client == NULL)
	{
		log_info("  - User %s not exist,buffer", buffer);               // user not exist
		free(buffer);
		sprintf(sts_code,"%d",CLIENT_NOT_EXIT);                         // send code
		if ( send_string(sock_connect, sts_code) < 0 )
			return ERROR_EXC;
	}else if(data_client->status == STS_ONLINE)                         // user exist and status = online
	{	
		free(buffer);
		sprintf(sts_code,"%d",CLIENT_ONLINE);
		if ( send_string(sock_connect, sts_code) < 0 ) 
			return ERROR_EXC;
		sprintf(client_string,"%s %d",data_client->addr, data_client->udp_port);
		log_info("  - %s is online at '%s'", data_client->name, client_string);

    	if ( send_string(sock_connect, client_string) < 0)              // send recipient info to user
    		return ERROR_EXC;
	}else if(data_client->status == STS_OFFLINE){                       // user exist and status = offline
		log_info("  - %s is offline, msgs will be stored in the server", buffer);
		free(buffer);
		sprintf(sts_code,"%d",CLIENT_OFFLINE);
		if ( send_string(sock_connect, sts_code) < 0)
			return ERROR_EXC;		
		msg = rcv_string(sock_connect);                                 // rcv message to store in the queuee
		if ( msg == NULL)
			return ERROR_EXC;
		//init msg struct
		msg_t = malloc(sizeof(struct msg_entity));
		msg_t->msg 	= msg;
		msg_t->next = NULL;
		data_client->queuee_msg = add_msg_to_client(data_client->queuee_msg,msg_t);
	}
	return ALL_OK;
}

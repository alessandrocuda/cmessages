/*
 *
 *  Messanges Server side, msg_client
 * 	as part of the Model - view - controller pattern, this is the View.
 *
 *	Set up a tcp server listening on a port, passed as a parameter or deafult, 
 *  and responds to requests from clients.
 *  For each request creates a thread and the data are processed in mute exclusion with a mutex semaphore	
 *
 *	Usage: msg_server [OPTIONS]
 *  Messages Server
 *  	-p int       define tcp port server (default: 8282)
 *   	-h           print this help and exit
 *  	-v           run in verbose mode, for debug purpose
 */

#include "msg_server.h"
#include "server/arg_manage/arg_manage.h"
#include "server/controller_server.h"

struct 		reg_client* client_head;	// global data beetween threads
										// head linked list

static void msg_handler(int sts){
	switch(sts){
		case ALL_OK:
			log_info("request has been serverd");
			break;
		case ERROR_EXC:
			log_info("request has not been serverd for some error: %s",strerror(errno));
			break;

	}
}


void 
*listen_thread_fun(void *sock)
{	
	int 	sock_connect = *((int *) sock);
	char 	*cmd_buff;
	int 	ret = 0;

	log_debug(" - read operation user\n");
	set_controller_data(sock_connect, &client_head);
	cmd_buff = read_command();
	if ( strcmp(cmd_buff, "!register") == 0)
		ret = register_command();
	else if (strcmp(cmd_buff, "!who") == 0)
		ret = who_command();
	else if(strcmp(cmd_buff, "!quit") == 0)
		ret = quit_command();
	else if (strcmp(cmd_buff, "!deregister") == 0)
 		ret = deregister_command();
    else if(strcmp(cmd_buff, "!send") == 0)
    	ret = send_command();
    msg_handler(ret);

	free(cmd_buff);
	close(sock_connect);
	pthread_exit(NULL);	
}


int 
main(int argc, char  **argv)
{	FILE 		*server_log_file;
	int 		tcp_port;
	int 		listen_socket;
	struct 		sockaddr_in server_addr, client_addr;

	pthread_t 	listen_thread;

	/*
	 * Set default behavior
	 */

	server_log_file = fopen("server_log", "a+");
	if ( server_log_file < 0){
		log_error("error open file");
	}
	log_set_quiet(QUIET_ON);	 // log off
	log_set_fp(server_log_file);
	tcp_port = DEFAULT_PORT; 
	/*
	 * Set flag
	 */
	init_argv(&tcp_port, argc, argv);


	log_info("|---------- Start Server ---------|");
	log_info(" - server run on port: %d", tcp_port);
	listen_socket = create_server(&server_addr, tcp_port);
	init_reg_client(&client_head);
	while(1)
	{
		log_debug(" - wait for a client request on listen socket: %d", tcp_port);
		int len = sizeof(client_addr);
		int *new_sd = malloc(sizeof(*new_sd));

   		*new_sd = accept(listen_socket, (struct sockaddr*) &client_addr, (socklen_t*) &len);
		if(new_sd < 0)
		{
			log_fatal(" - ACCEPT ERRROR: %s",strerror(errno));
        	exit(-1);
    	}

    	pthread_create(&listen_thread, NULL, listen_thread_fun, (void *) new_sd);
	}
	fclose(server_log_file);
	close(listen_socket);
	log_info("|---------- Quit Server ---------|");
	pthread_exit(NULL);	
}



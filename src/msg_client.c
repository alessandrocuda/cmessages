/*
 *  Messanges Client side, msg_client
 * 	as part of the Model - view - controller pattern, this is the View.
 *	Messages Client
 *	Usage: msg [OPTIONS]
 *    	-s   ip:port    define src client ip and port (default: localhost:8283) - UDP
 *   					E.g. 127.0.0.1:2222, 192.168.1.2
 *   					port can be set only if you set an ip first
 * 	
 *  	-d   ip:port    define dst server ip and port (default: localhost:8282) - TCP\n", stdout);
 *  					E.g. 127.0.0.1:2222, 192.168.1.2
 *  					port can be set only if you set an ip first\n", stdout);
 *      
 *      -h              print this help and exit\n", stdout);
 *  	-v              run in verbose mode, for debug purpose\n", stdout);
 *	
 *	Guide:
 *  When Messanges Client start, this following commands are available:
 *		!help                  show commands list
 *	   	!register username     registers the client at the server, 'username' must be min 3 to max 15,
 *			                   only alphanumeric chars, - / _ and no space
 *		!deregister            de-register client from server (you must be online)
 *		!who                   show clients list available
 *		!send username         send a message to another client
 *		!quit                  set client offline and exit!
 */


#include "msg_client.h"
#include "log/log.h"
#include "utils/model_data.h"
#include "client/arg_manage/arg_client.h"
#include "client/controller_client.h"

struct         client_entity     client = CLIENT_DEFAULT();
struct         server_entity     server = SERVER_DEFAULT();

static void
hello_message( struct client_entity *client, struct server_entity *server)
{
	fputs(" -------------------------------------------------\n", stdout);
	fputs("|             Welcome To Message Client           |\n",stdout);
	fputs(" -------------------------------------------------\n", stdout);
	fprintf(stdout, " - Client setting: %s:%d\n", client->addr,client->udp_port);
	fprintf(stdout, " - Server setting: %s:%d\n", server->addr,server->tcp_port);

}


static void msg_handler(int sts){
	char *result;
	switch(sts){
		case RESULT_SETTED:
			log_info("command exec ok");
			result = get_result_controller();
			fprintf(stdout, "%s\n",result );
			free(result);
			break;
		case ERROR_CONNECT_TO_SERVER:
			fprintf(stderr, "- CONNECT TO SERVER ERROR: %s\n",strerror(errno) );
			break;
		case ALREADY_REG:
			fprintf(stdout, "You are already registered with username: %s\n",client.name);
			break;
		case DEREG_SUCCEEDED:
			fprintf(stdout, "You have been deregistered\n");
			break;
		case REG_SUCCEEDED:
			fprintf(stdout, "Registration succeeded for %s\n", client.name);
			break;
		case REG_USER_WITH_SAME_NAME:
			fprintf(stdout, "An user is online with the same username\nChoose another one.\n");
			break;
		case USER_WHERE_OFFLINE:
			fprintf(stdout, "Welcome back user: %s\n",client.name);
			result = get_result_controller();
			if(  result != NULL )
				fprintf(stdout, "%s\n", result);
			else
				fputs("you have no messages in pending\n", stdout);
			free(result);
			break;
		case NOT_REGISTERD:
			fprintf(stdout, "You are not registered\n");
			break;
		case CANNOT_SEND_MSG_TO_YOU:
			fprintf(stdout, "You can't send messages to yourself\n");
			break;
		case CLIENT_NOT_FOUND:
			fprintf(stdout, "Client not found!\n");
			break;
		case CLIENT_GO_OFFLINE:	
			fprintf(stdout, "User %s go offline and quit\n",client.name );
			fputs("Close, bye bye!\n", stdout);
			break;
		case ERROR_EXC:
			fprintf(stderr," - try again, command has failed!:\n   %s\n", strerror(errno));
			break;
	}
	fflush(stdout);
	return;
}

/*
 *	Overide ctrl-c and SIGKILL behavior
 *	User go offline if registred and close client
 */
static void handler_singint(int signum)
{
	int ret;
	init_controller("!quit", NULL);
    ret = set_offline_command(server, client);
    fputs("\n", stdout);
    msg_handler(ret);
    exit(EXIT_SUCCESS);
}


int 
main(int argc, char  **argv)
{		
	char 		cmd_buff[BUFFER_SIZE];
	regex_t 	regex;
	int 		ret;


	pthread_t 	listen_thread;
    struct sigaction sa;									 

	/*
	 * Set default behavior
	 */

	log_set_quiet(QUIET_ON);	 // log off by deafult
    sa.sa_handler = handler_singint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; /* Restart functions if
                               interrupted by handler */
    if (sigaction(SIGINT, &sa, NULL) == -1)
        log_info("Cant' set up ctrl+c handler");
    ret = regcomp(&regex, REGEX_USERNAME, REG_EXTENDED);
	if (ret) {
		fprintf(stderr, "Could not compile regex\n");
		exit(EXIT_FAILURE);
	}
	init_argv(&client, &server, argc, argv);

	log_info("|---------- Run Client ----------|");
	hello_message(&client, &server);
	pthread_create(&listen_thread, NULL, live_msgs, (void *) &client);


    while(1){
		// waint user command
	    char *cmd;											// store command
		char *cmd_context="";								// store command arg

		fprintf(stdout, "%s > ", client.is_reg ? client.name : "");
		fflush(stdin);
		fgets(cmd_buff, BUFFER_SIZE, stdin);
		cmd = strtok_r(cmd_buff, " ", &cmd_context);

		if (cmd[strlen(cmd)-1] == '\n')
		{
			log_info("remove");
			cmd[strlen(cmd)-1]='\0';
		}
		init_controller(cmd, cmd_context);

		/*
		 *	Execute the right Command
		 */
	    if ( strcmp(cmd, "!help") == 0){
	    	/*
			 *	Execute !help Command
			 */
	    	printf("%s",HELP);
	    	continue;
	    }else if ( strcmp(cmd, "!who") == 0){
	    	/*
			 *	Execute !who Command
			 */
	    	fputs("Show Clients List\n",stdout);
	    	ret = who_command(server);
	    	msg_handler(ret);
	    	continue;
	    }else if ( strcmp(cmd, "!quit") == 0){
	    	/*
			 *	Execute !quit Command
			 */
	    	ret = set_offline_command(server, client);
	    	msg_handler(ret);
	    	goto exit;
	    }else if ( strcmp(cmd, "!register" ) == 0){
	    	/*
		 	 *	!Register Command
		 	 */
			/* Compile regular expression */
	    	if (cmd_context==NULL)
	    	{
	    		fprintf(stdout, "You must specify an username \n");
		    	continue;
	    	}

			if( cmd_context[strlen(cmd_context)-1]=='\n')
				cmd_context[strlen(cmd_context)-1]='\0';
			ret = regexec(&regex, cmd_context, 0, NULL, 0);
			if (ret == REG_NOMATCH)
			{
				fprintf(stdout, "Use a correct name\n");
				fprintf(stdout, " - '%s' cannot be use as username\n",cmd_context);
				fprintf(stdout, "- see !help command for more detail\n");
				continue;
			}
	    	ret = register_command(server,&client);
	    	msg_handler(ret);
	    	continue;
	    }else if ( strcmp(cmd, "!deregister") == 0){
	    	ret = deregister_command(server, &client);
	    	msg_handler(ret);
			continue;
	    }else if ( strcmp(cmd, "!send") == 0){
	    	/*
		 	 *	!Send Command
		 	 */
	    	if (cmd_context==NULL)
	    	{
	    		fprintf(stdout, "You must specify an username \n");
		    	continue;
	    	}
	    	if( cmd_context[strlen(cmd_context)-1]=='\n')
				cmd_context[strlen(cmd_context)-1]='\0';
			ret = regexec(&regex, cmd_context, 0, NULL, 0);
			if (ret == REG_NOMATCH)
			{
				fprintf(stdout, "Use a correct name\n");
				continue;
			}
			ret = send_command(server, &client);
			msg_handler(ret);

	    }else{
	    	fprintf(stdout,"Command not found, type !help for the command list\n");
	    	continue;
	    }
	}

	exit:
		regfree(&regex);
		log_info("|---------- Quit Client ---------|");
		exit(EXIT_SUCCESS);
}



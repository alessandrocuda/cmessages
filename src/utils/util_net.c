/*
 * Connectivity module, provides a high-level attraction for the following features:
 *  -   Create a TCP Server
 *  -   Connect to a TCP server
 *  -   Send a string by an open TCP connection
 *  -   Receives a string from an open TCP connection
 *  -   Create a UDP Socket 
 *  -   Send a instant message using a UDP Socket 
 */
#include "util_net.h"


/* 
 * ----------------------------------------------------------------- 
 *|                         Create a TCP Server
 * ----------------------------------------------------------------- 
 * \param   *sockaddr  init sockaddr_in passed as arg
 * \param   port       port to sent in *sockaddr
 * \return  sd         sodket file descriptor if no error occurred
 */
int
create_server(struct sockaddr_in *sockaddr, const int port)
{
	// Creazione socket
	int ret;
	int sd = socket(AF_INET, SOCK_STREAM, 0);

	memset(sockaddr, 0, sizeof(*sockaddr));
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = htons(port);
	sockaddr->sin_addr.s_addr = INADDR_ANY;

	ret = bind(sd ,(struct sockaddr*) sockaddr, sizeof(*sockaddr));
	if(ret < 0)
	{
		log_fatal(" - BIND ERRROR: %s",strerror(errno));
        exit(EXIT_FAILURE);
    }

	ret = listen(sd, 10);
	if(ret < 0)
	{
		log_fatal(" - LISTEN ERRROR: %s",strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sd;
}

/* 
 * ----------------------------------------------------------------- 
 *|                       Connect to a TCP server
 * ----------------------------------------------------------------- 
 * \param   *sockaddr  init sockaddr_in passed as arg
 * \param   server     server_entity, store information about server
 * \return  sd         socket file descriptor if no error occurred
 */
int
connect_to_server(struct sockaddr_in *sockaddr, struct server_entity server ){
	int ret;
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	memset(sockaddr, 0, sizeof(*sockaddr));
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = htons(server.tcp_port);
	ret = inet_pton(AF_INET, server.addr, &sockaddr->sin_addr);
    if (ret != 1 )
    {
        return -1;
    }
    log_info("connect to server: %s:%d", server.addr,server.tcp_port);
	ret = connect(sd, (struct sockaddr*) sockaddr, sizeof(*sockaddr));
    if(ret < 0){
       return -1;
    }
    return sd;
}

/* 
 * ----------------------------------------------------------------- 
 *|           Receives a string from an open TCP connection
 * ----------------------------------------------------------------- 
 * \param   sd         socket file descriptor, must be init with 
 *                     int connect_to_server(...)
 * \return  buff       return a string, buff is dynamically allocated
 *                     be carefull to manage memory with this char 
 *                     pointer   
 */

char*
rcv_string(int sd){
	int ret;
	int len;
	char *buff;
	uint32_t lmsg;
	ret = recv(sd, (void*)&lmsg, sizeof(uint32_t), 0);
    if(ret < 0){
    	log_fatal(" - RCV DIM ERRROR: %s",strerror(errno));
        return NULL;	
    }	
    len = ntohs(lmsg);
    buff = (char *) malloc(sizeof(char)*len);
    if(len == 0){
    	//server cannot read 0 byte, send error message to client, manage it!
        free(buff);
    	return NULL;
    }
    log_debug(" - command msg dim: %d\n",len);
    ret = recv(sd, (void*) buff, len, 0);
    if(ret < 0){
    	log_fatal(" - RCV DATA ERRROR: %s",strerror(errno));
        free(buff);
		return NULL;	
    }

    return buff;
}


/* 
 * ----------------------------------------------------------------- 
 *|            Send a string by an open TCP connection
 * ----------------------------------------------------------------- 
 * \param   sd         socket file descriptor, must be init with 
 *                     int connect_to_server(...)
 * \return  buff       return a string, buff is dynamically allocated
 *                     be carefull to manage memory with this char 
 *                     pointer   
 */
int
send_string(int sd, char *str){
	int 		ret;
	int 		len;
	uint32_t 	lmsg;
	len = strlen(str) + 1;
	lmsg = htons(len);
	ret = send(sd, (void*) &lmsg, sizeof(uint32_t), 0);
    if (ret < sizeof(uint32_t)) {
        perror("Errore in fase di send dimensione: \n");
        return -1;
    }
    ret = send(sd, (void*) str, len, 0);
    if (ret < len) {
        perror("Errore in fase di send di una stringa: \n");
        return -1;
    }
    return 0;
}

/* 
 * ----------------------------------------------------------------- 
 *|                      Create a UDP Socket
 * ----------------------------------------------------------------- 
 * \param   *sockaddr  init sockaddr_in passed as arg
 * \param   *ip        ip to bind
 * \par     port       on which port to set the UDP Socket
 * \return  sd         socket file descriptor if no error occurred
 */

int
create_udp_socket(struct sockaddr_in *sockaddr, char *ip, int port){
    int sd;
    int ret;
    /* Creazione indirizzo di bind */
    sd = socket(AF_INET,SOCK_DGRAM,0);

    memset(sockaddr, 0, sizeof(*sockaddr)); // Pulizia 
    sockaddr->sin_family = AF_INET;
    sockaddr->sin_port = htons(port);
    log_info("create udp socket at %s:%d",ip,port);
    ret = inet_pton(AF_INET, ip, &sockaddr->sin_addr);
    if (ret != 1 )
    {
        perror("- IP IS NOT VALID");
        exit(EXIT_FAILURE);
    }
    ret = bind(sd, (struct sockaddr*) sockaddr, sizeof(*sockaddr) );
    if( ret < 0 ){
        perror("- BIND CLIENT UDP PORT FAIL\n");
        exit(EXIT_FAILURE);
    }  
    return sd;
}


/* 
 * ----------------------------------------------------------------- 
 *|            Send a instant message using a UDP Socket 
 * ----------------------------------------------------------------- 
 * \param   *mgs       message to send 
 * \param   *ip        what ip do you want to send the message
 * \param   port       select the port 
 */
void
send_instant_msg(char* msgs, char *ip, int port){
    struct  sockaddr_in client_dst;
    int     sd;
    int     ret;

    memset(&client_dst, 0, sizeof(client_dst)); // Pulizia 
    client_dst.sin_family = AF_INET;
    client_dst.sin_port = htons(port);
    log_info("create udp socket at %s:%d",ip,port);
    ret = inet_pton(AF_INET, ip, &client_dst.sin_addr);
    if (ret != 1 )
    {
        perror("- IP IS NOT VALID");
        return;
    }

    sd = socket(AF_INET,SOCK_DGRAM,0);
    ret = sendto(sd, msgs, strlen(msgs)*sizeof(char), 0, (struct sockaddr*)&client_dst, sizeof(client_dst));
    if ( ret < 0){
        perror("- try again, message has not been sent");
        return;
    }

    close(sd);

}

/* 
 * ----------------------------------------------------------------- 
 *|                  check if the port is valid 
 * ----------------------------------------------------------------- 
 * \param   port       port to valid 
 * \return  bool       return true (valid) or false (not valid)
 */

bool
is_valid_port(int port){
    if (port>=1 && port<=65535)
        return true;
    return false;
}
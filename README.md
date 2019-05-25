# cmessage
A client-server and P2P messaging application written in C

## Description

This is a "messaging" system that allows the exchange of messages between different users.

The system implements two modes of message exchange: instant messages and offline messages:

 - **Instant message** exchange takes place directly between users via P2P interactions and via UDP sockets, this mode can only be used when users are in conversation and are both online.

 - **Offline messages**, it takes place via client/server communication via TCP socket and will be used when the user is not online. In this mode the server will memorize the message and transmit it when the recipient user will be online again.

The server have to memorizing the data of registered users and, among these, which ones are currently online. For online users the server is storing the IP address and UDP port on which the client is listening in order to provide information to other clients that want to send a message in instant mode. The exchange of information between client and server will take place via TCP sockets. The server port and IP address will be provided at the command line when the client is launched.

## Table of Contents 
- [Stack](#stack)
- [Usage](#usage)
- [Build](#build)
- [Todo](#todo)
- [Support](#support)
- [License](#license)


## Stack
- [log.c](https://github.com/rxi/log.c): A simple logging library implemented in C99
- **pthread**: POSIX Threads system call
- **socket api**: create an endpoint for communication linux system call
- **MVC**: Model–View–Controller (usually known as MVC) as architectural pattern

## Usage
  
        ipc1_src:portc1_src            ip_server:port_server          ipc2_src:portc2_src
           ------------                      --------                     ------------    
      |-> |  Client 1  |<-----------------> | Server |<----------------->|  Client 2  |<-| 
      |    ------------   re/degistration    --------                     ------------   |
      |                   Offline Messages                                               |
      |             retrive clients information                                          |
       ----------------------------------------------------------------------------------
                                                            Instant Messages


### Client

Messages Client side, msg_client.out

    Usage: msg_client.out [OPTIONS]
    	
    -s   ip:port    define src client ip and port (default: localhost:8283) - UDP
	    			E.g. 127.0.0.1:2222, 192.168.1.2
		    		port can be set only if you set an ip first

    -d   ip:port    define dst server ip and port (default: localhost:8282) - TCP\n", stdout);
  					E.g. 127.0.0.1:2222, 192.168.1.2
  					port can be set only if you set an ip first\n", stdout);
      
    -h              print this help and exit\n", stdout);
  	-v              run in verbose mode, for debug purpose\n", stdout);

When Messanges Client start, this following commands are available:

    !help                   show commands list
    !register username      registers the client at the server, 'username' must be min 3 to  
                            max 15, only alphanumeric chars, - / _ and no space
    !deregister             de-register client from server (you must be online)
    !who                    show clients list available
    !send username          send a message to another client
    !quit                   set client offline and exit!

### Server

Messanges Server side, msg_server.out
Set up a tcp server listening on a port, passed as a parameter or deafult, and responds to requests from clients.
For each request creates a thread and the data are processed in mute exclusion with a mutex semaphore	

    Usage: msg_server [OPTIONS]
    Messages Server
    	-p int       define tcp port server (default: 8282)
     	-h           print this help and exit
    	-v           run in verbose mode, for debug purpose

## BUILD
```bash
git clone https://github.com/alessandrocuda/cmessages
cd cmessages
make
```
### Run
```bash
#if you want some help just type:
./msg_client.out -h
./msg_server.out -h

#run cmessages client
./msg_client.out -s client_ip:client_port -d server_ip:server_port 
#run cmessages server
./msg_server.out  -p server_port     
```
### Examples
Some example will be add soon!

## TODO
- [ ] add some Examples to README.md

## Support
Reach out to me at one of the following places!

- Website at <a href="https://alessandrocudazzo.it" target="_blank">Alessandro Cudazzo</a>.
- Twitter at <a href="http://twitter.com/alessandrocuda" target="_blank">`@alessandrocuda`</a>

## License
[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

This library is free software; you can redistribute it and/or modify it under
the terms of the MIT license. 

- **[MIT license](LICENSE)**
- Copyright 2019 © <a href="https://alessandrocudazzo.it" target="_blank">Alessandro Cudazzo</a>.
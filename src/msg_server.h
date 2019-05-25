#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "log/log.h"
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>

#define CLIENT_VERSION 1.0.0

#define BUFFER_SIZE 	1024
#define DEFAULT_PORT 	8282
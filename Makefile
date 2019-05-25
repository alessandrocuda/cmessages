#
# 'make depend' 		uses makedepend to automatically generate dependencies 
#               		(dependencies are added to end of Makefile)
# 'make'        		build executable file 'msg_client' and 'msg_server'
# 'make msg_client'     build executable file 'msg_client' 
# 'make msg_server'     build executable file 'msg_server'

# 'make clean'  		removes all .o 
# 'make clean_all'  	removes all .o and executable files
#

# define C compiler to use
CC = gcc

# define any compile-time flags
CFLAGS = -Wall -g

# define any directories containing header files other than /usr/include
#
INCLUDES =

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = 

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = -lpthread

# define the C source files
SOURCEDIR := ./src/
#COM_SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
COM_SOURCES	:= $(SOURCEDIR)/log/log.c $(SOURCEDIR)/utils/util_net.c $(SOURCEDIR)/utils/model_data.c
CLIENT_SRC	:= $(SOURCEDIR)/msg_client.c  $(SOURCEDIR)/client/arg_manage/arg_client.c  $(SOURCEDIR)/client/controller_client.c
SERVER_SRC	:= $(SOURCEDIR)/msg_server.c  $(SOURCEDIR)/server/arg_manage/arg_manage.c  $(SOURCEDIR)/server/controller_server.c
# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
COM_OBJS = $(COM_SOURCES:.c=.o)
CLIENT_OBJS = $(CLIENT_SRC:.c=.o)
SERVER_OBJS = $(SERVER_SRC:.c=.o)
ALL_OBJ = $(COM_OBJS) $(CLIENT_OBJS) $(SERVER_OBJS)

# define the executable file 
CLIENT = msg_client.out
SERVER = msg_server.out

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean clean_all

all:    $(CLIENT) $(SERVER)
		@echo Messange Server and Client are been compiled

$(CLIENT): $(COM_OBJS) $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(CLIENT) $(COM_OBJS) $(CLIENT_OBJS) $(LFLAGS) $(LIBS)
$(SERVER): $(COM_OBJS) $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(SERVER) $(COM_OBJS) $(SERVER_OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
./log/log.o: ./log/log.c
	$(CC) $(CFLAGS) -DLOG_USE_COLOR -c $<  -o $@

clean:
#	$(RM) -v *.o *~ $(MAIN) $(ALL_OBJ)
	$(RM) -v $(ALL_OBJ)
clean_all:
	$(RM) -v $(MAIN) $(ALL_OBJ) $(CLIENT) $(SERVER)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
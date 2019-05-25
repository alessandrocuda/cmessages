#include "arg_manage.h"


static void
usage(const char *name)
{
    fprintf(stdout, "Usage: %s [OPTIONS]\n", name);
    fputs("Messages Server\n", stdout);
    fputs("\n", stdout);
    fputs("-p int       define tcp port server (default: 8282)\n", stdout);
    fputs("-h           print this help and exit\n", stdout);
    fputs("-v           run in verbose mode, for debug purpose\n", stdout);
}

void
init_argv(int* port, const int argc, char  **argv){
		int c; 					// temp arg for getopt, GNU c lib std
		while ((c = getopt(argc, argv, "p:hv")) != -1){
		switch (c){
			case 'p':
				*port = atoi(optarg);
				break;
			case 'v':
				log_set_quiet(QUIET_OFF);
				log_set_level(LOG_INFO);
				break;
			case 'h':
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
				if (optopt == 'p'){
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
    						 usage(argv[0]);
    			}
				else if (isprint (optopt)){
				  	fprintf (stderr, 
				  			 "Unknown option `-%c'.\n", optopt);
							 usage(argv[0]);
				}
				else{
					//char is not printable
				  	fprintf (stderr,
				    	     "Unknown option character `\\x%x'.\n",
				             optopt);
					usage(argv[0]);
				}
				exit(EXIT_FAILURE);
			default:
				abort ();
		}
	}

}
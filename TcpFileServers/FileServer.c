#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>


void error(const char *msg)
{
	perror(msg);
	printf ("%s", strerror(errno));
	exit(1);
}

void IterativeServer(){

}

void ConMulProc(){

}

void ConMulThread(){

}

void ConPreFork(){

}

void ConPreThread(){

}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t client_length;
	char buffer[BUFSIZ];
	struct sockaddr_in cli_addr;

	if(argc < 2){
		error("Invalid arguments\n"
				"./FileServer <server type number>\n"
				"1 for Iterative\n"
				"2 for Concurrent multi-processor with one process per request\n"
				"3 for Concurrent multi-threaded server with one thread per request\n"
				"4 for Concurrent pre-forked multi-processing server\n"
				"5 for Concurrent pre-threaded multi-threading server");
	}

	char* serverType = argv[1];
	switch(*serverType){
	case '1':
		IterativeServer();
		break;
	case '2':
		ConMulProc();
		break;
	case '3':
		ConMulThread();
		break;
	case '4':
		ConPreFork();
		break;
	case '5':
		ConPreThread();
		break;
	default:
		error("Type of server, out of range");
	}
}

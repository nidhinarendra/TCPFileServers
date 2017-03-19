// TCP file server with concurrent multiprocessing


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>


/*
 **********************************************************************************************************
 Function Name : error
 Purpose	   : Function to display the error message from the program to standard error.
 Parameter     : character pointer which points to the starting of the message
 Return Value  : The program exits with the condition 1
 **********************************************************************************************************
 */

void error(const char *msg)
{
	perror(msg);
	printf ("%s", strerror(errno));
	exit(1);
}

/*
 **********************************************************************************************************
 Function Name : Main
 Purpose	   : Program starts here
 Parameter     : None
 Return Value  : Integer
 *********************************************************************************************************
 */
void *connection_handler (void *sockfd) {

	int MY_BUFSIZ = 1024*10;
	char buffer[MY_BUFSIZ];
	struct sockaddr_in serv_addr, cli_addr;
	pid_t childpid;
	int sizeSentToClient = -1;
	int newsockfd = *(int*)sockfd;
	int rwSuccess;
	// To compute the length of the client socket
	socklen_t client_length;

	bzero(buffer,MY_BUFSIZ);

	//Read the message sent by the client
	rwSuccess = recvfrom(newsockfd, buffer, MY_BUFSIZ, 0, (struct sockaddr *) &cli_addr, &client_length);
	if (rwSuccess < 0) error("ERROR reading from socket");
	printf("The file requested is:%s\n", buffer);

	char send_buf[MY_BUFSIZ]; /* max chunk size for sending file */

	FILE *fp;
	fp = fopen (buffer, "r");
	if(fp == NULL)
	{
		error("%s File NOT FOUND!");
		exit(1);
	}

	else{
		printf("Sending file: %s\n", buffer);
		if (fgets(send_buf, MY_BUFSIZ,fp)  != NULL)
		{
			int len_file_read_into_buffer = strlen(send_buf);
			printf("Sending length:%lu  \n", strlen(send_buf));
			if( (sizeSentToClient = sendto(newsockfd, send_buf, len_file_read_into_buffer, 0,
					(struct sockaddr *)&cli_addr, sizeof(struct sockaddr_in) ))
					< len_file_read_into_buffer )
			{
				error("send error");
				return -1;
			}
		}
		else{
			error("Error reading the file");
		}
	}
	fclose(fp);

	printf("Length of the file sent is %d bytes\n\n",
			(int)sizeSentToClient);

	return 0;

}


int main(int argc, char **argv)
{
	//sockfd and newsockfd are the file descriptors
	//rwSuccess is the return value for read() and write() calls
	int sockfd, newsockfd, portno, rwSuccess;

	// To compute the length of the client socket
	socklen_t client_length;

	//The characters written on the socket is read into this buffer
	int MY_BUFSIZ = 1024*10;
	char buffer[MY_BUFSIZ];
	struct sockaddr_in serv_addr, cli_addr;
	pid_t childpid;
	int sizeSentToClient = -1;

	if(argc < 2){
		printf("Invalid arguments\n"
				"usage: ./%s <port number>", argv[0]);
	}

	portno = atoi(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	//Binding socket to an address and checking if it is successful
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	printf("The server is listening on port %d\n", portno);

	listen(sockfd,5);

	for(;;){
		memset(&cli_addr, '\0', sizeof(cli_addr));
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &client_length);
		if (newsockfd < 0)
			error("ERROR on accept");

		pthread_t newThread;
		if(pthread_create(&newThread, NULL, connection_handler, (void*) &newsockfd) <0 ){
			error("Could not create thread");
			return 1;
		}



	}
}

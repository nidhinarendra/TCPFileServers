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


int main(int argc, char **argv)
{
	int sockfd, newsockfd, portno, rwSuccess;
	socklen_t client_length;
	char buffer[BUFSIZ];
	struct sockaddr_in serv_addr, cli_addr;
	pid_t childpid;
	int sizeSentToClient = -1;

	if(argc < 2){
		error("Invalid arguments\n"
				"usage: ./conmulthread <port number>");
	}

	portno = atoi(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	printf("The server is listening on port %d\n", portno);

	listen(sockfd,5);


	for(;;){
		memset(&cli_addr, '\0', sizeof(cli_addr));
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &client_length);
		if (newsockfd < 0)
			error("ERROR on accept");

		//Creating a child process to proceed with the new client
		if ((childpid = fork()) == -1) {
			close(newsockfd);
		}
		/*
		else if(childpid > 0){
			close(newsockfd);
		}
		else if(childpid == 0)
		 */
		{
			bzero(buffer,BUFSIZ);

			//Read the message sent by the client
			rwSuccess = recvfrom(newsockfd, buffer, BUFSIZ, 0, (struct sockaddr *) &cli_addr, &client_length);
			if (rwSuccess < 0) error("ERROR reading from socket");
			printf("The file requested is:%s\n", buffer);

			int sent_count; /* how many sending chunks, for debugging */
			ssize_t read_bytes, /* bytes read from local file */
			sent_bytes, /* bytes sent to connected socket */
			sent_file_size;

			char send_buf[BUFSIZ]; /* max chunk size for sending file */

			FILE *fp;
			int f;
			fp = fopen (buffer, "r");
			if(fp == NULL)
			{
				error("%s File NOT FOUND!");
				exit(1);
			}

			else{
				printf("Sending file: %s\n", buffer);
				if (fgets(send_buf, BUFSIZ,fp)  != NULL)
				{
					int len_file_read_into_buffer = strlen(send_buf);
					printf("Sending length:%d data:%s \n", strlen(send_buf), send_buf);
					if( (sizeSentToClient = sendto(newsockfd, send_buf, len_file_read_into_buffer, 0,
							(struct sockaddr *)&cli_addr, sizeof(struct sockaddr_in) ))
							< len_file_read_into_buffer )
					{
						error("send error");
						return -1;
					}
				}
				else{
					printf("had problem reading the data from the file");
				}

			}

			close(f);
			fclose(fp);
			printf("Done with this client. Sent %d bytes\n\n",
					(int)sizeSentToClient);


			//Close the child when the task is completed
			close(newsockfd);

		}
	}
}

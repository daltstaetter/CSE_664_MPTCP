#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 20000 
#define BACKLOG 5
#define LENGTH 2048 
#define TIMEOUT_IN_SECONDS	8
#define SOL_TCP 6 // as defined in netinet/tcp.h

#define IP_ADDRESS_SERVER_WLAN0 "10.231.232.192"
#define IP_ADDRESS_SERVER_ETHO  "10.0.0.2"
#define IP_ADDRESS_SERVER        IP_ADDRESS_SERVER_WLAN0

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main (int argc, char* argv[])
{
	/* Defining Variables */
	int sockfd; 
	int nsockfd; 
	int num;
	int sin_size; 
	int status;
	int option = 2;
	struct sockaddr_in addr_local; /* client addr */
	struct sockaddr_in addr_remote; /* server addr */
	char revbuf[LENGTH]; // Receiver buffer
	char *fr_name;
	FILE *fr;

	/* Get the Socket file descriptor */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor. (errno = %d)\n", errno);
		exit(1);
	}
	else 
	{
		printf("[Server] Obtaining socket descriptor successfully.\n");
	}

	status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));
	if (status < 0) 
		error("Error reusing port and address. Exiting...");

	struct timeval timeout = {TIMEOUT_IN_SECONDS, 0};
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const void*) &timeout, sizeof(timeout)) < 0)
    		fprintf(stderr, "Error configuring socket for rcv-timeout (errno = %d)\n", errno);


	
	/* Fill the client socket address struct */
	addr_local.sin_family = AF_INET; // Protocol Family
	addr_local.sin_port = htons(PORT); // Port number
	addr_local.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER); // Server IP Address
	bzero(&(addr_local.sin_zero), 8); // Flush the rest of struct

	/* Bind a special Port */
	if( bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to bind Port. (errno = %d)\n", errno);
		exit(1);
	}
	else 
		printf("[Server] Binded tcp port %d in addr 10.230.152.34 sucessfully.\n",PORT);

	/* Listen remote connect/calling */
	if(listen(sockfd,BACKLOG) == -1)
	{
		fprintf(stderr, "ERROR: Failed to listen Port. (errno = %d)\n", errno);
		exit(1);
	}
	else
	{
		printf ("[Server] Listening the port %d successfully.\n", PORT);
	}

	int success = 0;
	while(success == 0)
	{
		sin_size = sizeof(struct sockaddr_in);

		/* Wait a connection, and obtain a new socket file descriptor for single connection */
		if ((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, &sin_size)) == -1) 
		{
		    fprintf(stderr, "ERROR: Obtaining new Socket Descriptor. (errno = %03d)\n", errno);
			exit(1);
		}
		else 
		{
			printf("[Server] Server has got connected from %s.\n", inet_ntoa(addr_remote.sin_addr));
		}

		/*Receive File from Client */
		fr_name = "/home/csce/Tex_Project/CSE_664_MPTCP/File_send_recv/file_received_from_client.txt";

		if (argc > 1)
			fr = fopen(fr_name, argv[1]); // can be "w" (overwrite) or "a" (append)
		else
			fr = fopen(fr_name, "w");

		if(fr == NULL)
		{
			printf("File %s Cannot be opened on server.\n", fr_name);
		}
		else
		{
			bzero(revbuf, LENGTH); 
			int fr_block_sz = 0;
			while((fr_block_sz = recv(nsockfd, revbuf, LENGTH, 0)) > 0) 
			{
			    	int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
				if(write_sz < fr_block_sz)
			    	{
			        	error("File write failed on server.\n");
			    	}
			 	
				//printf("frblock_sz: %d\n", fr_block_sz);
	
				bzero(revbuf, LENGTH);
				//if (fr_block_sz == 0 || fr_block_sz != LENGTH) 
				{
				//	printf("Breaking out...%d\n", fr_block_sz);
				//	break;
				}
			}
			
			if(fr_block_sz < 0)
			{
		        	if (errno == EAGAIN)
	        		{
	                		printf("recv() timed out.\n");
	            		}
	            		else
	            		{
	                		fprintf(stderr, "recv() failed due to errno = %d\n", errno);
					exit(1);
	            		}
        		}
			
			printf("Ok received from client!\n");
			fclose(fr); 
			close(nsockfd);
		}
	}
}

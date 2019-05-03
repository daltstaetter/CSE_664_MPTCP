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
#define LENGTH 2048

#define IP_ADDRESS_SERVER_WLAN0 "10.231.232.192"
#define IP_ADDRESS_SERVER_ETH0  "10.0.0.2"
#define IP_ADDRESS_SERVER       IP_ADDRESS_SERVER_WLAN0

#define SOL_TCP         6   // as defined in include/linux/socket.h
#define MPTCP_ENABLED   42  // as defined in include/uapi/linux/tcp.h

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	/* Variable Definition */
	int sockfd; 
	int nsockfd;
	char revbuf[LENGTH]; 
	struct sockaddr_in remote_addr;
	char* fs_name; 
    int mptcp_enable = 0;

	/* Get the Socket file descriptor */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor! (errno = %d)\n",errno);
		exit(1);
	}

    setsockopt(sockfd, SOL_TCP, MPTCP_ENABLED, &mptcp_enable, sizeof(mptcp_enable));

	/* Fill the socket address struct */
	remote_addr.sin_family = AF_INET; 
	remote_addr.sin_port = htons(PORT); 
	inet_pton(AF_INET, IP_ADDRESS_SERVER, &remote_addr.sin_addr); // could make IP an argument
	bzero(&(remote_addr.sin_zero), 8);

	/* Try to connect the remote */
	if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
		exit(1);
	}
	else 
		printf("[Client] Connected to server at port %d...ok!\n", PORT);

	/* Send File to Server */
	//if(!fork())
	//{
		if (argc > 1)
			fs_name = argv[1]; //"/home/csce/Tex_Project/CSE_664_MPTCP/File_send_recv/mptcp_2016_04_18.tar.gz"; // could make fname an arg
		else
			fs_name = "/home/csce/Tex_Project/CSE_664_MPTCP/File_send_recv/file_sent_to_server.txt"; // could make fname an arg
			//char* fs_name = "/home/csce/Tex_Project/CSE_664_MPTCP/File_send_recv/mptcp_2016_04_18.tar.gz"; // could make fname an arg

		char sdbuf[LENGTH]; 
		printf("[Client] Sending %s to the Server... \n", fs_name);
		FILE *fs = fopen(fs_name, "r");
		if(fs == NULL)
		{
			printf("ERROR: File %s not found.\n", fs_name);
			exit(1);
		}

		bzero(sdbuf, LENGTH); 
		int fs_block_sz; 
		while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
		{
		    if(send(sockfd, sdbuf, fs_block_sz, 0) < 0)
		    {
		        fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
		        break;
		    }
		    bzero(sdbuf, LENGTH);
		}
		printf("Ok File %s from Client was Sent!\n", fs_name);
	//}

	// Receive File from Server //
//	printf("[Client] Receiving file from Server and saving it as final.txt...");
//	char* fr_name = "/home/csce/Tex_Project/CSE_664_MPTCP/File_send_recv/file_from_client.txt";
//	FILE *fr = fopen(fr_name, "a");
//	if(fr == NULL)
	{
//		printf("File %s Cannot be opened.\n", fr_name);
	}
//	else
	{
/*		bzero(revbuf, LENGTH); 
		int fr_block_sz = 0;
	    	while((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0)
	    	{
			int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
	        	if(write_sz < fr_block_sz)
			{
	            		error("File write failed.\n");
	        	}
			
			bzero(revbuf, LENGTH);
			
			if (fr_block_sz == 0 || fr_block_sz != LENGTH) 
			{
				break;
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
			}
		}
*/	    
		printf("Ok received from server!\n");
//	    	fclose(fr);
	}
	
	close (sockfd);
	printf("[Client] Connection lost.\n");
	
	return (0);
}

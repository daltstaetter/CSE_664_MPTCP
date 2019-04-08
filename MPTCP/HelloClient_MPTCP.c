// HelloClient.c

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#include <netinet/tcp.h> 	// SOL_TCP
#include <linux/tcp.h> 	// MPTCP_* socket options

#include <string.h>
#include <time.h>

#define ERROR 			-1
#define TCP_PORT		10000
#define TCP_IP			"127.0.0.1"
#define TCP_IP0_VM1		"192.168.56.102"
#define TCP_IP1_VM1		"192.168.57.4"
#define BUFF_SIZE		1024


//-----------------------------------------------------
#define SOL_TCP              			6 // as defined in netinet/tcp.h
#define ENABLED							1
//-----------------------------------------------------


void Error_Check(int input, const char *err_msg)
{
	if (input == ERROR)
	{	
		perror(err_msg);
		exit(ERROR);
	}
}


int main(void)
{
	struct sockaddr_in server;
	int socket_fd;

	int recv_bytes, sent_bytes;
	
	char* hello = "Hello from Client!!!";
	char recv_buffer[BUFF_SIZE] = {0};
	int status;
//	clock_t t_start, t_end;
//	double CPU_time;
//-----------------------------------------------------

	int mptcp_enable = ENABLED;
//-----------------------------------------------------

	memset(&server, 0, sizeof(server));	
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	Error_Check(socket_fd, "socket: ");


//-----------------------------------------------------
	setsockopt(socket_fd, SOL_TCP, MPTCP_ENABLED, &mptcp_enable, sizeof(mptcp_enable));
	Error_Check(status, "setsockopt: MPTCP_ENABLED ");	
	


//-----------------------------------------------------	
	server.sin_family = AF_INET;
	server.sin_port = htons(TCP_PORT);
	//server.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("127.0.0.1");
	//server.sin_addr.s_addr = htonl(inet_addr("127.0.0.1"));
	status = inet_pton(AF_INET, TCP_IP0_VM1, &server.sin_addr);
	// pton = "presentation to network", map strings to bytes in network order
	Error_Check(status, "inet_pton: ");

	status = connect(socket_fd, (struct sockaddr *) &server, sizeof(server));
	Error_Check(status, "connect: ");

// Not sure if to check this before or after the 'connect()' call
	getsockopt(socket_fd, SOL_TCP, MPTCP_ENABLED, &mptcp_enable, (socklen_t *) sizeof(mptcp_enable));
	printf("Is MPTCP_ENABLED: %d\n", mptcp_enable);

//	t_start = clock();
	sent_bytes = send(socket_fd, hello, strlen(hello), 0);
	recv_bytes = recv(socket_fd, recv_buffer, BUFF_SIZE, 0);
	//t_end = clock();

//	CPU_time = ((double)(t_end - t_start)) / CLOCKS_PER_SEC;
	printf("Hello Msg Sent to %s:%d!\n", "192.168.56.102", TCP_PORT);
	printf("%s", recv_buffer);
//	printf("Took %f seconds\n\n", CPU_time);
}

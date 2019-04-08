// HelloServer.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 	// structures
#include <errno.h> 			// perror
#include <string.h> 		// strlen
#include <unistd.h> 		// close

//#include <netinet/tcp.h> 	// SOL_TCP
#include <linux/tcp.h> 	// MPTCP_* socket options
//#include <net/mptcp.h>  // not sure if I should be explicitly adding this header, ADD_ADDR

#include <arpa/inet.h> 		// inet_ntoa
#include <time.h>

#define ERROR		-1
#define TCP_PORT 	10000
#define TCP_IP 		"127.0.0.1"
#define TCP_IP0_VM1	"192.168.56.102"
#define TCP_IP1_VM1	"192.168.57.4"
#define BUFF_SIZE	1024

//-----------------------------------------------------
#define SOL_TCP              			6 // as defined in netinet/tcp.h
#define MPTCP_INFO_FLAG_SAVE_MASTER 	0x01 // stores initial subflow info even if it closed
#define ENABLED							1
//-----------------------------------------------------

// 1. Create socket
// 2. Bind socket to specific IP:TCP_PORT
// 3. Wait for clients (Listen/Accept)
// 4. After client connects we wait for Recieve mesg "Hello World" from client
// 5. Close connection

void Error_Check(int input, const char *err_msg)
{
	if (input <= ERROR)
	{	
		perror(err_msg);
		exit(ERROR);
	}
}

void main(void)
{
	int socket_fd;
	struct sockaddr_in server, client;
	unsigned int len;
	int sent_bytes, recv_bytes, status;
	int option = 2;
	char recv_buffer[BUFF_SIZE] = {0}; 

	int opened_socket;
	char returned_msg[] = "Hello Back From Server!\n";
	clock_t t_start, t_end;
	double CPU_time;

//-----------------------------------------------------
	int mptcp_capable;
	int val = MPTCP_INFO_FLAG_SAVE_MASTER; // info from inital subflow is stored even if it closed
	int mptcp_enable = ENABLED;

	// Setup struct for MPTCP Info
	struct mptcp_info minfo;
	struct mptcp_meta_info meta_info;
	struct tcp_info initial;
	struct tcp_info others[3];
	struct mptcp_sub_info others_info[3];

	minfo.tcp_info_len = sizeof(struct tcp_info);
	minfo.sub_len = sizeof(others);
	minfo.meta_len = sizeof(struct mptcp_meta_info);
	minfo.meta_info = &meta_info;
	minfo.initial = &initial;
	minfo.subflows = (struct tcp_info *) &others;
	minfo.sub_info_len = sizeof(struct mptcp_sub_info);
	minfo.total_sub_info_len = sizeof(others_info);
	minfo.subflow_info = (struct mptcp_sub_info *) &others_info;
	

//-----------------------------------------------------

	// see 'man socket' for API and required headers
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	Error_Check(socket_fd, "socket: ");

	status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));
	Error_Check(status, "setsockopt: REUSE(ADDR|PORT) ");

//-----------------------------------------------------
	setsockopt(socket_fd, SOL_TCP, MPTCP_ENABLED, &mptcp_enable, sizeof(mptcp_enable));
	Error_Check(status, "setsockopt: MPTCP_ENABLED ");	

	char *pathmanager = "fullmesh"; // {default, fullmesh, ndiffports, binder}
    	status = setsockopt(socket_fd, SOL_TCP, MPTCP_PATH_MANAGER, pathmanager, sizeof(pathmanager));
	Error_Check(status, "setsockopt: MPTCP_PATH_MANAGER ");
	
	//char *scheduler = "redundant"; // {default, roundrobin, redundant}
	//status = setsockopt(socket_fd, SOL_TCP, MPTCP_SCHEDULER, scheduler, sizeof(scheduler));
	//Error_Check(status, "setsockopt: MPTCP_SCHEDULER ");

	status = setsockopt(socket_fd, SOL_TCP, MPTCP_INFO, &val, sizeof(val));
	Error_Check(status, "setsockopt: MPTCP_INFO_FLAG ");

	getsockopt(socket_fd, SOL_TCP, MPTCP_INFO, &minfo, (socklen_t *) sizeof(minfo));
	//getsockopt(socket_fd, SOL_SOCKET, MPTCP_INFO, &minfo, (socklen_t *) sizeof(minfo));

	getsockopt(socket_fd, SOL_TCP, MPTCP_ENABLED, &mptcp_enable, (socklen_t *) sizeof(mptcp_enable));
	printf("Is MPTCP_ENABLED: %d\n", mptcp_enable);

//-----------------------------------------------------

	server.sin_family = AF_INET;
	server.sin_port = htons(TCP_PORT);
	server.sin_addr.s_addr = inet_addr(TCP_IP0_VM1);
	//server.sin_addr.s_addr = htonl(INADDR_ANY);


	// Next, bind server info to the specified socket/address
	status = bind(socket_fd, (struct sockaddr *) &server, sizeof(server));
	Error_Check(status, "bind: ");

	// Next, we listen with 5 allowed connections
	status = listen(socket_fd, 5);
	Error_Check(status, "listen: ");

	while(1)
	{
		// Next, we accept the client connections from the queue and store its information
		opened_socket = accept(socket_fd, (struct sockaddr *)&client, (socklen_t *)&len);
		Error_Check(opened_socket, "accept: ");		

// Not sure which socket to use to get the MPTCP_Enable value
		getsockopt(socket_fd, SOL_TCP, MPTCP_ENABLED, &mptcp_enable, (socklen_t *) sizeof(mptcp_enable));
		printf("Is MPTCP_ENABLED: %d\n", mptcp_enable);

		getsockopt(opened_socket, SOL_TCP, MPTCP_ENABLED, &mptcp_enable, (socklen_t *) sizeof(mptcp_enable));
		printf("Is MPTCP_ENABLED: %d\n", mptcp_enable);	
//

		t_start = clock();
		recv_bytes = recv(opened_socket, recv_buffer, BUFF_SIZE, 0);
		sent_bytes = send(opened_socket, returned_msg, strlen(returned_msg), 0);
		t_end = clock();

		CPU_time = ((double)(t_end - t_start)) / CLOCKS_PER_SEC;
		printf("%s\n", recv_buffer);
		printf("Hello message sent\n"); 
		printf("Sent %d bytes to client: %s:%d\n", sent_bytes, inet_ntoa(client.sin_addr), TCP_PORT);     
		printf("Took %f seconds\n\n", CPU_time);
		
		close(opened_socket);
	}
}


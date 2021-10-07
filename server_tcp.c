#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <errno.h>   		/* Error number definitions */

#define MAX_RECV_LEN 4096
#define PORT 9996

fd_set readfds;		//set of socket descriptors
int max_sd;
int client_socket[2];
int	isTcpC=0x00;
int master_socket , addrlen , new_socket   , activity, valread , sd;
int opt = 1;
struct timeval timeout;
struct sockaddr_in address; 

int socket_desc , client_sock , c , read_size;
struct sockaddr_in server , client;
char client_message[4096];
int yes=1;

void wait4Connection()
{
start_again:
	//clear the socket set
	FD_ZERO(&readfds);

	//add master socket to set
	FD_SET(master_socket, &readfds);
	max_sd = master_socket;
	 
	//socket descriptor
	sd = client_socket[0];
	 
	//if valid socket descriptor then add to read list
	if(sd > 0)
		FD_SET( sd , &readfds);
	 
	//highest file descriptor number, need it for the select function
	if(sd > max_sd)
		max_sd = sd;
			
	//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
	activity = select( max_sd + 1 , &readfds , NULL , NULL , &timeout);
   // printf("\nACT=%d\n",activity); fflush(stdout);
	if( activity == -1)	
		goto start_again;
	
	if ((activity < 0) && (errno!=EINTR))
		printf("select error");
			  
	//If something happened on the master socket , then its an incoming connection
	if (FD_ISSET(master_socket, &readfds))
	{
		if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
		{
			perror("accept");
			exit(1);
		}
	   
		isTcpC = 1;
	   
		close(master_socket);

		//if position is empty
		if( client_socket[0] == 0 )
		{
			client_socket[0] = new_socket;
		}
	}
}

int initTcpServer(void)
{
	//initialise all client_socket[] to 0 so not checked
   
    client_socket[0] = 0;
      
    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        return (1);
        //exit(1);
    }
  
    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        return (1);
        //exit(1);
    }
  
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
     
    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        return (1);
    }
    printf("Listener on port %d \n", PORT);
     
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        return (1);
    }
      
    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");
    
    return EXIT_SUCCESS;
}

void process_TcpServer(void)
{ 
	sd = client_socket[0];
	
	valread = recv(sd,client_message,MAX_RECV_LEN,MSG_DONTWAIT);
	if (valread == 0)
	{
		isTcpC = 0;
		
		//Somebody disconnected , get his details and print
		getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
		
		printf("\nHost disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); fflush(stdout);
	   
		close(sd);
		client_socket[0] = 0;
		
		initTcpServer();
	}
	else if( valread > 0 )
	{
		printf("\nclient_message : %s\n",client_message); fflush(stdout);
		
		memset(client_message,0x00,MAX_RECV_LEN);
	}
}

int main()
{
	initTcpServer();
	
	while(1)
	{
		if(isTcpC==0x00)
		{
			wait4Connection();
		}
		
		//Reader as a server
		if(isTcpC==1)
			process_TcpServer();
	}
	
	return 0;
}

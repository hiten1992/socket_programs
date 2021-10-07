// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
  
#define PORT     9996
#define MAXLINE 1024
#define IP_ADRR "192.168.100.98"
  
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char message[1000];
    struct sockaddr_in     servaddr;
  
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
  
    memset(&servaddr, 0, sizeof(servaddr));
      
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(IP_ADRR);
      
    int n, len;
    
    while(1)
    {
        printf("Enter message : ");
        gets(message);
        
        if(sendto(sockfd, (const char *)message, strlen(message),MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        {
            printf("\nsend failed\n"); fflush(stdout);
            exit(1);
        }
        
    }
    
    close(sockfd);
    return 0;
}

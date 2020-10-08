
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "arguments.h"
#include "serverMessage.h"

#define PORT 10000
#define MAXLINE 1024

int sockfd, nsent;
struct arguments *udp_args;

int udpClientConnect(void *args)
{
    udp_args = (struct arguments *)args;
    int n;
    struct sockaddr_in servaddr;

    // clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;
    if (inet_aton(udp_args->ip_address, &servaddr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    // create datagram socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // connect to server
    if (n = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("\n Error : Connection Failed\n");
    }
    else if (n == 0)
    {

        printf("udp client connected ip: %s port: %d. you can connect netcat with command: netcat -l -u -p 10000, to get inotify details.\n", udp_args->ip_address, PORT);
    }

    return 0;
}

int sendMessageToRemoteServer(char *filename, char *access_type, char *time)
{
    char toSend[5000] = {'\0'};
    sprintf(toSend, "FILE ACCESSED: %s/%s\nACCESS:%s\nDATE:%s\n", udp_args->directory, filename, access_type,time);
    puts(toSend); 
    if ((nsent = send(sockfd, toSend, strlen(toSend), 0)) < 0)
        perror("Error");
    return 0;
}

int closeSocket()
{
        close(sockfd);

}
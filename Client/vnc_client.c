#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <stdbool.h>
#include "/Users/rigved/Programs/RDP/Client/meta_info.h"


#define MAX 256
#define PORT 8080
#define SA struct sockaddr
#define IPADDR "192.168.0.7"
struct fb_meta_fix fixed_info_server;
struct fb_meta_var var_info_server;


void handleError(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

bool closeVNC(char* buff){
    if (strncmp(buff, "exit", 4) == 0) {
        printf("Client Exit...\n");
        return true;
    }
    return false;
}

void getInfo(int sockfd, char* buff, char* infoVal){
    memset(buff, 0, sizeof(buff));
    printf("Client: requested for fixed info");
    buff = infoVal; // if infoVal is fixed, then gets fixed meta data
    // Send to server
    (write(sockfd, buff, strlen(buff)));

    // Clear buffer and read response
    memset(buff, 0, sizeof(buff));
    if(strcmp(infoVal, "fixed") == 0){
        if (read(sockfd, &fixed_info_server, sizeof(fixed_info_server)) < 0) {
            handleError("error in getting fixed info");
        }
    }
    else if(strcmp(infoVal, "var") == 0){
        if (read(sockfd, &var_info_server, sizeof(var_info_server)) < 0) {
            handleError("error in getting variable info");
        }
    }
    printf("Server: sent info\n");
}

void chat(int sockfd)
{
    char buff[MAX];
    int n;
    while(!closeVNC(buff)){
        getInfo(sockfd, buff, "fixed");
        getInfo(sockfd, buff, "var");
        
    }
}

void init(int* sockfd, struct sockaddr_in* servaddr)
{
    // socket create and verification
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(servaddr, sizeof(*servaddr));

    // assign IP, PORT
    servaddr->sin_family = AF_INET;
    servaddr->sin_addr.s_addr = inet_addr(IPADDR);
    servaddr->sin_port = htons(PORT);
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    init(&sockfd, &servaddr);
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        printf("Connecting to %s", IPADDR);
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // function for chat
    chat(sockfd);

    // close the socket
    close(sockfd);
}
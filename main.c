//
//  main.c
//  JZHTTPD
//
//  Created by Jerry Zhang on 21/11/2017.
//  Copyright © 2017 Jerry Zhang. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>
//int acceptRequest(int serverSock);
int serverStartUp(u_int16_t *port);
void errorDie(char * stirng);
int acceptRequest(char * buffer,char * response);
int serverStartUp(u_short *port) {
    int createSocket = 0;
    struct sockaddr_in name;
    //socklen_t nameLen = sizeof(name);
    
    int on = 1;
    createSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(createSocket == -1)
        errorDie("Setting up socket");
    
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    name.sin_port = htons(*port);
    
    if((setsockopt(createSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
        //setsockopt() manipulate the options associated with a socket.
        errorDie("Setting sock opt");
    if(bind(createSocket, (struct sockaddr *)&name , sizeof(name)) < 0)
        errorDie("Binding");
    if(listen(createSocket, 5) < 0)
        errorDie("Setting up listen");
    printf("Server startup succeeded.\n");
    return createSocket;
}

void errorDie(char * stirng) {
    printf("%s failure!\n",stirng); //TODO: change to perror?
    exit(-1);
}

int acceptRequest(char * buffer,char * response) {
    return 1;
}
int main(int argc, const char * argv[]) {
    char *buffer;
    int buffLen = 1024 * sizeof(char);
    int responseLen = 1024 * sizeof(char);
    char *response = (char *)malloc(responseLen);
    buffer = malloc(buffLen);
    u_short port = 15000;
    int serverSock = serverStartUp(&port);
    struct sockaddr_in client;
    socklen_t clientLen = sizeof(client);
    printf("JZHTTPD running on %u\n",port);
    while(1) {
        int clientSocket = accept(serverSock,(struct sockaddr *)&client, &clientLen);
        if(clientSocket < 0)
            errorDie("Client socket");
        if(clientSocket > 0)
            printf("Client conneted\n");
        recv(clientSocket, buffer, buffLen, 0);
        printf("%s",buffer);
        acceptRequest(buffer, response);
        write(clientSocket, "Hello World from JZHTTPD!\n", sizeof("Hello World from JZHTTPD!\n"));
        close(clientSocket);
    }
    return 0;
}

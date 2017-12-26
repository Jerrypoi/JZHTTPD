//
//  main.c
//  JZHTTPD
//
//  Created by Jerry Zhang on 21/11/2017.
//  Copyright © 2017 Jerry Zhang. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>//send , socket;
#include <sys/types.h>
#include <netinet/in.h>// sockaddr_in
//#include <arpa/inet.h>
#include <unistd.h> //write();
#include <ctype.h>
#include <strings.h>
#include <string.h>
//#include <sys/wait.h>
#include <stdlib.h>
//#include <stdint.h>
#define SERVER_STRING "Server: Jerry's WEBSERVER Running JZHTTPD\r\n"
int serverStartUp(u_int16_t *port);
void errorDie(char * stirng);
void unimplemented(int client);
void notFound(int client);
void header(int client,FILE * file);
void sendFile(int client,FILE * file);
void sendFile(int client,FILE * file) {
    char usingString[1024];
    fgets(usingString, sizeof(usingString), file);
    while(!feof(file)) {
        write(client, usingString, strlen(usingString));
        fgets(usingString, sizeof(usingString), file);
    }
}
void header(int client,FILE * file) {
    char usingString[1024];
    strcpy(usingString, "HTTP/1.1 200 OK\r\n");
    write(client, usingString, strlen(usingString));
    
    write(client, SERVER_STRING, strlen(SERVER_STRING));
    
    strcpy(usingString, "Conten-Tpye: text/html\r\n");
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, "\r\n");
    write(client, usingString, strlen(usingString));
}
void notFound(int client) {
    char usingString[1024];
    strcpy(usingString, "HTTP/1.1 404 Not Found\r\n");
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, SERVER_STRING);
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, "Content-Type: text/html\r\n\r\n");
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, "<HTML><HEAD><TITLE>File Not Found</TITLE>\r\n");
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, "<BODY><H>404 NOT FOUND</H><P>Sorry, the file you requested cannot be found on this server.</P>\r\n");
    
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, "</BODY></HTML>\r\n");
    write(client, usingString, strlen(usingString));
}
void unimplemented(int client) {
    char usingString[1024];
    strcpy(usingString, "HTTP/1.0 501 Method Not Implemented\r\n");
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, SERVER_STRING);
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, "Content-Type: text/html\r\n\r\n");
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, "<HTML><HEAD><TITLE>Method Not Implemented</TITLE>\r\n");
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, "<BODY><H>501 Method UnImplemented</H><P>HTTP request method not supported.</P>\r\n");
    write(client, usingString, strlen(usingString));
    
    strcpy(usingString, "</BODY></HTML>\r\n");
    write(client, usingString, strlen(usingString));
    
}
//int acceptRequest(char * buffer,char * response);
int acceptRequest(char *buffer,int client) {
    char url[255];
    char method[255];
    
    int i = 0, j = 0;
    while(!isspace(buffer[i]) && i < sizeof(method) - 1) {
        method[i] = buffer[i];
        i++;
    }
    method[i] = '\0';
    while(isspace(buffer[i]))
        i++;
    while(!isspace(buffer[i]) && j < sizeof(url) - 1) {
        url[j++] = buffer[i++];
    }
    url[j] = '\0';
    if(strcmp(method, "GET") != 0) {
        unimplemented(client);
        return 0;
    }
    
    if(url[strlen(url) - 1] == '/')
        strcat(url, "index.html");
    sprintf(url, "www%s",url);
    FILE * file = fopen(url, "r");
    if(file == NULL) {
        notFound(client);
        return 0;
    }
    header(client, file);
    sendFile(client,file);
    return 1;
}

int serverStartUp(u_short *port) {
    int createSocket = 0;
    struct sockaddr_in localhost;
    int buff = 1;
    createSocket = socket(AF_INET,SOCK_STREAM, 0); //AF_INET : IPv4
    if(createSocket == -1)
        errorDie("Setting up socket");
    memset(&localhost, 0, sizeof(localhost));
    localhost.sin_family = AF_INET;
    localhost.sin_addr.s_addr = htonl(INADDR_ANY);//(u_int32_t)0x00000000
    localhost.sin_port = htons(*port);
    if((setsockopt(createSocket, SOL_SOCKET, SO_REUSEADDR, &buff, sizeof(buff))) < 0)
        //setsockopt() manipulates the options associated with a socket.
        errorDie("Setting sock opt");
    if(bind(createSocket, (struct sockaddr *)&localhost , sizeof(localhost)) < 0)
        errorDie("Binding");
    if(listen(createSocket, 5) < 0)//5 : Maximum pending connection.
        errorDie("Setting up listen");
    printf("Server startup succeeded.\n");
    return createSocket;
}

void errorDie(char * stirng) {
    printf("%s failure!\n",stirng); //TODO: change to perror?
    exit(-1);
}
int main(int argc, const char * argv[]) {
    char *buffer;
    int buffLen = 1024 * sizeof(char);
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
        acceptRequest(buffer,clientSocket);
        close(clientSocket);
    }
    return 0;
}

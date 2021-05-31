#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <dirent.h> 
#include <sys/stat.h>

#include "helpers.h"

int main(int argc, char **argv){

	int server_port, sockfd, listenfd, datafd, code, n5, n6, x;
    uint16_t port;
	struct sockaddr_in servaddr, data_addr;
	char command[1024], ip[50], str[MAXLINE+1];


	if(argc != 3){
		printf(RED "Invalid number of arguments. Please follow the format.\n");
		printf("Format: ./client <server-ip> <server-port>\n" RESET);
		exit(-1);
	}

	//get server port
	sscanf(argv[2], "%d", &server_port); // store argv[2] into server_port

    //set up control connection using sockfd as control socket descriptor
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    	perror("socket connection error occured\n");
    	exit(-1);
    }
                
    memset(&servaddr, '\0', (int)sizeof(servaddr));
    servaddr.sin_family = AF_INET; // address family -> trivially, we use AF_INET.
    servaddr.sin_port   = htons(server_port); // 16 bit port no, conv from network to host byte order

    // convert host addr into AF_INET addr family as store in server_addr.sin_addr
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
        // conversion successful only if inet_pton returns 1
    	perror("inet_pton error. Could not parse host address.");
    	exit(-1);
    }
        
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
    	perror("connection error occured.\n");
    	exit(-1);
    }


    // set up data connection
    listenfd = socket(AF_INET, SOCK_STREAM, 0); // TCP(SOCK_STREAM) will only break connection when one party exits or network error occurs

    memset(&data_addr, '\0', sizeof(data_addr));
    data_addr.sin_family      = AF_INET;
    data_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    data_addr.sin_port        = htons(0);

    // bind socket to the data_addr port
    bind(listenfd, (struct sockaddr*) &data_addr, sizeof(data_addr));

    // listen for connections on socket
    listen(listenfd, BACKLOGS);
    
    // get ip address from control port
    get_ip_port(sockfd, ip, (int *)&x);
    // printf("x: %d\n", x);
    printf(YEL "ip: %s\n" RESET, ip);
    //get data connection port from listenfd
    get_ip_port(listenfd, str, (int *)&port);
    
    printf(YEL "Port: %d\n" RESET,  port);
    printf(YEL "str: %s\n" RESET, str);
    convert(port, &n5, &n6);

    while(1){

        memset(command, '\0', strlen(command));
        
        // get prompt
        code = get_cmd_code(command);
        
        // case: quit
        if(code == 4){
            char quit[1024];
            sprintf(quit, "QUIT");
            write(sockfd, quit, strlen(quit));
            memset(quit, '\0', (int)sizeof(quit));
            read(sockfd,quit, 1024);
            printf(CYN "Server : %s\n" RESET, quit);
            break;
        }
        // printf("command: %s\n", command);

        //send PORT n1,n2,n3,n4,n5,n6
        memset(str, '\0', (int)sizeof(str));
        get_port_str(str, ip, n5, n6);

        write(sockfd, str, strlen(str));
        memset(str, '\0', (int)sizeof(str));
        datafd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        // printf("Data connection Established...\n");

        if(code == 1)
        {
            if(_ls(sockfd, datafd, command) < 0){
                close(datafd);
                continue;
            }
        }
        else if(code == 2){
            if(_get(sockfd, datafd, command) < 0){
                close(datafd);
                continue;
            }
        }
        else if(code == 5){
            if(_history(sockfd, datafd, command) < 0){
                close(datafd);
                continue;
            }
        }
        close(datafd);
    }
    close(sockfd);	
	return TRUE;
}

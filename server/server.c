#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "helpers.h"

int main(int argc, char **argv){

	int	listenfd, connfd, port;
	struct sockaddr_in	servaddr;
	pid_t pid;

	if(argc != 2){
		printf(RED "Invalid number of arguments. Please follow the format.\n");
		printf("Format: ./server <server-port>\n" RESET);
		exit(-1);
	}
	
	sscanf(argv[1], "%d", &port);
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, '\0', sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

	listen(listenfd, BACKLOGS);

	while(1){
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		printf(MAG "New client detected...\n" RESET);

		if((pid = fork()) == 0){
			close(listenfd);

			int datafd, code, x = 0, client_port = 0;
			char recvline[MAXLINE+1];
			char client_ip[50], command[1024];


			while(1){
				memset(recvline, '\0', (int)sizeof(recvline));
				memset(command, '\0', (int)sizeof(command));

				//get client's data connection port
    			if((x = read(connfd, recvline, MAXLINE)) < 0){
    				break;
    			}
    			printf("*****************\n%s \n", recvline);
                if(strcmp(recvline, "QUIT") == 0){
                    printf("Quitting...\n");
                    char goodbye[1024];
                    sprintf(goodbye,"221 Goodbye");
                    write(connfd, goodbye, strlen(goodbye));
                    close(connfd);
                    break;
                }
    			get_client_ip_port(recvline, client_ip, &client_port);

    			if((setup_data_connection(&datafd, client_ip, client_port, port)) < 0){
    				break;
    			}

    			if((x = read(connfd, command, MAXLINE)) < 0){
    				break;
    			}

    			printf("-----------------\n%s \n", command);

    			code = get_cmd_code(command);
    			if(code == 1){
    				_ls(connfd, datafd, command);
    			}else if(code == 2){
    				_get(connfd, datafd, command);
    			}else if(code == 4){
                    char reply[1024];
                    sprintf(reply, "550 Filename Does Not Exist");
                    write(connfd, reply, strlen(reply));
                    close(datafd);
                    continue;
                }               

    			close(datafd);
    			

			}
    		printf("Exiting Child Process...\n");
    		close(connfd);
    		_exit(1);
		}
		//end child process-------------------------------------------------------------
		close(connfd);
	}
}
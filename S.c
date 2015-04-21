#include <header.h>
void error(const char *msg, int i){
    perror(msg);
    exit(i);
}

int parseLine(char *line, char *command_array[]) {
	char *p;
	int count=0;
	p = strtok(line, " "); // divide string into different pieces
	while (p != 0 ) {
		command_array[count++] = p;
		p = strtok(NULL," ");// continue to divide the string if not empty
	}
	return count;
}

int new_client(int data_sock){
	listen(data_sock, 5);	//listen
	int data_client;
	struct sockaddr_in data_client_addr;
	int llen = sizeof(data_client_addr);
	data_client = accept(data_sock, (struct sockaddr*) &data_client_addr, (socklen_t *)&llen);
	return data_client;
}

void get_name(char *file_name, char *a){

	int j = 0; int i;
	for(i = 0; a[i] != '\r'; i++){
		file_name[j++] = a[i];
	} file_name[j] = 0;	

}

int main(int argc, char const *argv[])
{
	if(argc != 2) error("argument error!", 0);
	int sockfd;
	struct sockaddr_in serv_addr;
	int port_num = atoi(argv[1]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket\n", 2);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port_num);
    	bzero(&(serv_addr.sin_zero), 8); // Flush the rest of struct

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
       	error("ERROR on binding", 2);

	listen(sockfd,5);	//listen
	int client_sockfd;
	struct sockaddr_in client_addr;
	int len = sizeof(client_addr);
	char buf[BUFFERSIZE] = {0};
	// data socket
	int data_sock;
	int data_port_num = port_num;

	while(1){
		client_sockfd = accept(sockfd, (struct sockaddr*) &client_addr, (socklen_t *)&len);
		if (client_sockfd < 0) {printf("ERROR on accept"); continue;}
		sprintf(buf, "220 This is Dean Chen's Ftp Server!\n");
		send( client_sockfd, buf, (int)strlen(buf), 0);

		int count = 0;
		int i; 
		int ftp_mode = PORT_MODE; 
		
		while(1){  // enter loop
			bzero(buf, BUFFERSIZE);
			char *command_array[4] = {0};
	     	int result = read(client_sockfd, buf, BUFFERSIZE);
			printf("received cmd: %s", buf);
			if (result < 0) {printf("ERROR reading from socket\n"); close(client_sockfd); break;}
			result = 0;
			count = parseLine(buf, command_array);//拆分buf

            if(strcmp(buf, "NLST\r\n") == 0){
				
				system("ls > ../tmp"); // 将ls 结果读入tmp																						
				FILE *fp = fopen("../tmp", "r");
				fread(buf, sizeof(char), BUFFERSIZE, fp);
				fclose(fp);
				unlink("../tmp");	
				result = send( client_sockfd, buf, (int)strlen(buf), 0);
				
            }else if(strcmp(command_array[0], "CWD") == 0){
            	for(i = 0; command_array[1][i] != '\r'; i++); command_array[1][i] = '\0';
            	if(chdir(command_array[1]) == 0){
						char path[100];
						getcwd(path, 100);
						sprintf(buf,"250 change path succeed!\ncurrent path:\n%s\n",path);
					}
				else sprintf(buf, "450 ERROR wrong path\n");
				result = send( client_sockfd, buf, (int)strlen(buf), 0);


            }else if(strcmp(command_array[0], "DELE") == 0){
            	for(i = 0; command_array[1][i] != '\r'; i++); command_array[1][i] = '\0';
            	
				if(remove(command_array[1]) == 0) sprintf(buf, "250 DELE file succeed!\n");
				else sprintf(buf, "550 No such file!\n");
				result = send( client_sockfd, buf, (int)strlen(buf), 0);

            }else if(strcmp(buf, "PASV\r\n") == 0){
         		sprintf(buf, "227 %d\n",++data_port_num);
         		result = send( client_sockfd, buf, (int)strlen(buf), 0);
         		ftp_mode = PASV_MODE;
         		data_sock = socket(AF_INET, SOCK_STREAM, 0);
				if (data_sock < 0) error("ERROR opening data socket\n", 2);
         		serv_addr.sin_port = htons(data_port_num);
 				if (bind(data_sock, (struct sockaddr *) &serv_addr,
      					sizeof(serv_addr)) < 0) 
   					error("ERROR on binding", 2);
            }else if(strcmp(command_array[0], "STOR") == 0){
            	char file_name[100]; 
				get_name(file_name, command_array[1]);		
				
				if(ftp_mode != PASV_MODE){
					sprintf(buf, "425 Please send PASV first!\n");
					result = send( client_sockfd, buf, (int)strlen(buf), 0);
				}else{ // new a socket
					
					int data_client = new_client(data_sock);
					if (data_client < 0) {
						printf("ERROR on accept data sock!\n");
						sprintf(buf, "404 Error create data sock!\n");
						result = send( client_sockfd, buf, (int)strlen(buf), 0);
					}
					else{ // send file
						sprintf(buf, "150 start send file!\n");
						result = send( client_sockfd, buf, (int)strlen(buf), 0);

						ftp_get(data_client, file_name);
						sprintf(buf, "226 Transfer complete!\n");
						result = send( client_sockfd, buf, (int)strlen(buf), 0);
					}
					close(data_sock); //
					ftp_mode = PORT_MODE;
				}


            }else if(strcmp(command_array[0], "RETR") == 0){
				char file_name[100]; 
				get_name(file_name, command_array[1]);
				
				if(ftp_mode != PASV_MODE){
					sprintf(buf, "425 Please send PASV first!\n");
					result = send( client_sockfd, buf, (int)strlen(buf), 0);
				}else{ // new a socket
					
					int data_client = new_client(data_sock);
					if (data_client < 0) {
						printf("ERROR on accept data sock!\n");
						sprintf(buf, "404 Error send failed!\n");
						result = send( client_sockfd, buf, (int)strlen(buf), 0);
					}
					else{ // send file
						sprintf(buf, "150 start send file!\n");
						result = send( client_sockfd, buf, (int)strlen(buf), 0);
						result = ftp_put(data_client, file_name);
						if (result == 0){
							sprintf(buf, "550 No such file!\n");
							result = send( client_sockfd, buf, (int)strlen(buf), 0);
						}else {
							sprintf(buf, "226 Transfer complete!\n");
							result = send( client_sockfd, buf, (int)strlen(buf), 0);
						}
					}
					close(data_sock); //
					ftp_mode = PORT_MODE;
				}

            }else if(strcmp(command_array[0], "USER") == 0){
            	char file_name[100]; 
				get_name(file_name, command_array[1]);
				sprintf(buf, "220 Welcome %s no pass required!\n", file_name);
				result = send(client_sockfd, buf, (int)strlen(buf), 0);
				
            }else if(strcmp(command_array[0], "PASS") == 0){
				char file_name[100]; 
				get_name(file_name, command_array[1]);
				sprintf(buf, "220  %s won't be your pass!\n", file_name);
				result = send(client_sockfd, buf, (int)strlen(buf), 0);

            }else if(strcmp(buf, "BYE\r\n") == 0){
            	sprintf(buf, "221 good night!\n");
				result = send(client_sockfd, buf, (int)strlen(buf), 0);
            	close(client_sockfd);
            	break;

            }else{
            	sprintf(buf, "500 Invalid wrong command\n");
				result = send( client_sockfd, buf, (int)strlen(buf), 0);
            }

    		if (result < 0) {
    			close(client_sockfd);
    			printf("ERROR writing to socket!\n");
			}


	  	 }//while
	}

	close(sockfd);
	return 0;
}


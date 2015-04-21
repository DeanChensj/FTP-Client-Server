#ifndef FUNCTION_H
#define FUNCTION_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <time.h> 
#include <pthread.h>
#include <ctype.h>
#include <sys/time.h>
#include <wait.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#define BUFFERSIZE     512
#define FTP_BUF_SIZE  1024 
#define PASV_MODE 0
#define PORT_MODE 1

int ftp_get(int soc, char* file_name);
int ftp_put(int soc, char* file_name);
int get_port(char *buffer_read);
int get_port2(char *buffer_read);

#endif

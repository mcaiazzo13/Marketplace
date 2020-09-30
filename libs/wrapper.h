#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

/* -----------------------
	-- WRAPPED FUNCTION
   -----------------------*/


int Socket(int,int, int);
int Inet_pton (int,const char *,void *);
int Connect(int,const struct sockaddr *,socklen_t);
int Bind(int,const struct sockaddr *,socklen_t);
int Listen(int,int);
int Accept(int,struct sockaddr *,socklen_t *);
int Close(int );
int Setsockopt (int, int, int, const void *, socklen_t);
pid_t Fork();



int Socket(int famiglia,int tipo,int protocollo){
	int ret;
	if((ret = socket(famiglia,tipo,protocollo)) < 0){
		perror("Error socker");
		exit(1);
	}
	return ret;
}

int Inet_pton(int famiglia,const char * servaddr,void* servaddr_len){
	int ret;
	if((ret = inet_pton(famiglia,servaddr,servaddr_len)) <= 0){
		perror("Error inet_pton");
		exit(1);
	}
	return ret;
}

int Connect(int sockfd,const struct sockaddr *servaddr,socklen_t servaddr_len){
	int ret;
	if((ret = connect(sockfd,servaddr,servaddr_len))< 0){
		perror("Error connect");
		exit(1);
	}
	return ret;
}

int Bind(int sockfd,const struct sockaddr *servaddr,socklen_t servaddr_len){
	int ret;
	if((ret = bind(sockfd,servaddr,servaddr_len))< 0){
		perror("Error bind");
		exit(1);
	}
	return ret;
}

int Listen(int sockfd,int size_coda){
	int ret;
	if((ret = listen(sockfd,size_coda)) < 0){
		perror("Error listen");
		exit(1);
	}
	return ret;
}

int Accept(int sockfd,struct sockaddr *servaddr,socklen_t *servaddr_len){
	int ret = accept(sockfd,servaddr,servaddr_len);
	if(ret == ENOBUFS || ret == ENOMEM || ret == EPROTO){
		perror("Error accept");
		exit(1);
	}
	
	return ret;
}

int Close(int sockfd){
	int ret;
	if((ret = close(sockfd))< 0){
		perror("Error close");
		exit(1);
	}
	
	return ret;
}

int Setsockopt( int sockfd, int level, int optname, const void *optval, socklen_t optlen ){
	int ret;
	if((ret = setsockopt ( sockfd, level, optname, optval, optlen) < 0)){
		perror ( "Error setsockot");
		exit(1);
	}
	return ret;
}


pid_t Fork(){
	pid_t pid;
	pid = fork();
	if(pid<0){
		perror("Error fork");
		exit(-1);
	}
	return pid;
}


// this is server N
// read from client N -> write to Master -> read from Master -> write to client N

#include "lib.h"

int main(){
	// utility 
	int listenfd,serverfd,connfd;
	int enable = 1;
	int choice,res;
	Store store; //store
	Product product; // product
	User user;
	struct sockaddr_in serverN,serverM,clientN;
	
	// socket 
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	Setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&enable,sizeof(enable));
	
	// set server ip,port and socket domain
	serverN.sin_addr.s_addr= htonl(INADDR_ANY);
	serverN.sin_port= htons(1025);
	serverN.sin_family= AF_INET;
	
	// bind
	Bind(listenfd,(struct sockaddr*) &serverN,sizeof(serverN));
	
	// listening
	Listen(listenfd,10);
	for(;;){
		int sizeN = sizeof(clientN);
		
		// accept connection with client N
		connfd = Accept(listenfd,(struct sockaddr*)&clientN,&sizeN);
		
		printf("Connection established with client N\n");
		
		// create a child process to handle communication
		if(!Fork()){
			// close listenfd
			Close(listenfd);
			
			// reading from connfd socket (clientN)
			FullRead(connfd,&choice,sizeof(choice));
			FullRead(connfd,&user,sizeof(user));
			switch(choice){
				case 1: // login
					        // connect to server Master
                                                serverfd = Socket(AF_INET,SOCK_STREAM,0);
                                                serverM.sin_addr.s_addr = inet_addr("127.0.0.1");
                                                serverM.sin_port = htons(1024);
                                                serverM.sin_family = AF_INET;

                                                Connect(serverfd,(struct sockaddr*)&serverM,sizeof(serverM));
                                                printf("Connected to server Master\n");

                                                // write to Master the choice
                                                FullWrite(serverfd,&choice,sizeof(choice));
						FullWrite(serverfd,&user,sizeof(user));

                                                // read from master
                                                FullRead(serverfd,&res,sizeof(res));
                                                // write response to client
                                                FullWrite(connfd,&res,sizeof(res));

						//Close(connfd);
						read(serverfd,&choice,sizeof(choice));
						Close(serverfd);

					break;
				case 2: // registration
					        // connect to server Master
                                                serverfd = Socket(AF_INET,SOCK_STREAM,0);
                                                serverM.sin_addr.s_addr = inet_addr("127.0.0.1");
                                                serverM.sin_port = htons(1024);
                                                serverM.sin_family = AF_INET;

                                                Connect(serverfd,(struct sockaddr*)&serverM,sizeof(serverM));
                                                printf("Connected to server Master\n");

                                                // write to Master the choice
                                                FullWrite(serverfd,&choice,sizeof(choice));
						FullWrite(serverfd,&user,sizeof(user));

                                                // read from master
                                                FullRead(serverfd,&res,sizeof(res));
                                                // write response to client
                                                FullWrite(connfd,&res,sizeof(res));

						//Close(connfd);
						read(serverfd,&choice,sizeof(choice));
						Close(serverfd);

					break;
				case 3: // add store
					{
						int ret;
						// connection to server M
						serverfd = Socket(AF_INET,SOCK_STREAM,0);
						serverM.sin_addr.s_addr = inet_addr("127.0.0.1");
						serverM.sin_port= htons(1024);
						serverM.sin_family = AF_INET;
						
						Connect(serverfd,(struct sockaddr*) &serverM,sizeof(serverM));
						printf("Connection with server M established\n");
						sleep(1);	

						FullWrite(serverfd,&choice,sizeof(choice));
						FullRead (connfd,&store,sizeof(store));
						FullWrite(serverfd,&store,sizeof(store));
						FullRead (serverfd,&ret,sizeof(ret));
						FullWrite(connfd,&ret,sizeof(ret));
						
						//Close(connfd);
						read(serverfd,&choice,sizeof(choice));
						Close(serverfd);
					}
					break;
				case 4: // delete store
					{
						int ret;
						// connection to server M
						serverfd = Socket(AF_INET,SOCK_STREAM,0);
						serverM.sin_addr.s_addr = inet_addr("127.0.0.1");
						serverM.sin_port = htons(1024);
						serverM.sin_family= AF_INET;
						
						Connect(serverfd,(struct sockaddr*) &serverM,sizeof(serverM));
						printf("Connection with server M established\n");
						sleep(1);

						FullWrite(serverfd,&choice,sizeof(choice));
						FullRead(connfd,&store,sizeof(store));
						FullWrite(serverfd,&store,sizeof(store));
						FullWrite(serverfd,&user,sizeof(user));
						FullRead(serverfd,&ret,sizeof(ret));
						FullWrite(connfd,&ret,sizeof(ret));
						
						//Close(connfd);
						read(serverfd,&choice,sizeof(choice));
						Close(serverfd);
					}
					break;
				case 5:	// add product 
					{
						int ret;
						// connection to server M
						serverfd = Socket(AF_INET,SOCK_STREAM,0);
						serverM.sin_addr.s_addr = inet_addr("127.0.0.1");
						serverM.sin_port= htons(1024);
						serverM.sin_family= AF_INET;
						
						Connect(serverfd,(struct sockaddr*) &serverM,sizeof(serverM));
						printf("Connection with server M established\n");
						sleep(1);

						FullWrite(serverfd,&choice,sizeof(choice));
						FullRead(connfd,&product,sizeof(product));
						FullWrite(serverfd,&product,sizeof(product));
						FullWrite(serverfd,&user,sizeof(user));
						FullRead(serverfd,&ret,sizeof(ret));
						FullWrite(connfd,&ret,sizeof(ret));
						
						if(ret != 0)
							FullWrite(connfd,"Error add product\n",sizeof("Error add product\n"));
							
						//Close(connfd);
						read(serverfd,&choice,sizeof(choice));
						Close(serverfd);
					}
					break;
				case 6:	// delete product
					{
						int ret;
						// connection to server M
						serverfd = Socket(AF_INET,SOCK_STREAM,0);
						serverM.sin_addr.s_addr = inet_addr("127.0.0.1");
						serverM.sin_port= htons(1024);
						serverM.sin_family= AF_INET;
						
						Connect(serverfd,(struct sockaddr*) &serverM,sizeof(serverM));
						printf("Connection with server M established\n");
						sleep(1);

						FullWrite(serverfd,&choice,sizeof(choice));
						FullRead(connfd,&product,sizeof(product));
						FullWrite(serverfd,&product,sizeof(product));
						FullWrite(serverfd,&user,sizeof(user));
						FullRead(serverfd,&ret,sizeof(ret));
						FullWrite(connfd,&ret,sizeof(ret));
						
						if(ret != 0)
							FullWrite(connfd,"error product delete\n",sizeof("error product delete\n"));
							
						//Close(connfd);
						read(serverfd,&choice,sizeof(choice));
						Close(serverfd);
					}
					break;
			}// switch 
			exit(0);
		} // end child
		Close(connfd);
	} // for
	return 0;
}

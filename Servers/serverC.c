// this is server C 
// read from client C -> write to Master -> read from Master -> write to client C

#include "lib.h"

int main(){
	// utility
	int listenfd,connfd,serverfd;
	int enable = 1;
	int choice,x,res;
	Product prod;
	User user;
	struct sockaddr_in serverC,serverM,client;
	
	// listen socket
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	Setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&enable,sizeof(enable));
	
	// set sercer C
	serverC.sin_addr.s_addr = htonl(INADDR_ANY);
	serverC.sin_port= htons(1026);
	serverC.sin_family= AF_INET;
	
	// bind
	Bind(listenfd,(struct sockaddr *) &serverC,sizeof(serverC));
	
	// listening
	Listen(listenfd,10);
	
	for(;;){
		int sizeC = sizeof(client);
		int i;
		// accept connection from client
		connfd = Accept(listenfd,(struct sockaddr*)&client,&sizeC);
		
		printf("\nConnection established with ClientC \n");
		
		// create a child process to handle communication
		if(!Fork()){
			// closing listen socket
			close(listenfd);
			
			FullRead(connfd,&choice,sizeof(choice));
			FullRead(connfd,&user,sizeof(user));	

			switch(choice){
				case 1:{ // login 
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
				       }break;
				case 2:{ // register
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
				       }break;
				case 7: {// add product to the cart
						int ret,numProd,prod;
						char buffer[150] = { 0 };

						// connect to the server M
						serverfd = Socket(AF_INET,SOCK_STREAM,0);
						serverM.sin_addr.s_addr = inet_addr("127.0.0.1");
						serverM.sin_port= htons(1024);
						serverM.sin_family= AF_INET;
						
						Connect(serverfd,(struct sockaddr*) &serverM,sizeof(serverM));
						printf("Connected to server Master\n");
						
						FullWrite(serverfd,&choice,sizeof(choice));
						FullWrite(serverfd,&user,sizeof(user));
						
						FullRead(serverfd,&numProd,sizeof(numProd));
						FullWrite(connfd,&numProd,sizeof(numProd));
						if(numProd > 0){
						//	printf("nice there products\n");
							for(i = 0; i < numProd; i++){
								// read from server
								FullRead(serverfd,buffer,sizeof(buffer));
								// write to client
								FullWrite(connfd,buffer,sizeof(buffer));
							}
							FullRead(connfd,&prod,sizeof(prod));
							FullWrite(serverfd,&prod,sizeof(prod));
							FullRead(serverfd,&ret,sizeof(ret));
							FullWrite(connfd,&ret,sizeof(ret));	
						}		
						else{
							FullRead(serverfd,buffer,sizeof(buffer));
							FullWrite(connfd,buffer,sizeof(buffer));
							FullRead(connfd,&prod,sizeof(prod));
							FullWrite(serverfd,&prod,sizeof(prod));
							FullRead(serverfd,&ret,sizeof(ret));
							FullWrite(connfd,&ret,sizeof(ret));	
						}
						//Close(connfd);
						read(serverfd,&choice,sizeof(choice));
						Close(serverfd);
					}break;
				case 8: {// show cart
						int ret,numProd,prod;
						char buffer[150];

						// connection to server M
						serverfd = Socket(AF_INET,SOCK_STREAM,0);
						serverM.sin_addr.s_addr = inet_addr("127.0.0.1");
						serverM.sin_port= htons(1024);
						serverM.sin_family= AF_INET;
						
						Connect(serverfd,(struct sockaddr*) &serverM,sizeof(serverM));
						printf("Connected to server master\n");
						
						FullWrite(serverfd,&choice,sizeof(choice));
						FullWrite(serverfd,&user,sizeof(user));
						
						FullRead(serverfd,&numProd,sizeof(numProd));
						FullWrite(connfd,&numProd,sizeof(numProd));
						if(numProd > 0){
							for(i = 0; i < numProd; i++){
								FullRead(serverfd,buffer,sizeof(buffer));
								FullWrite(connfd,buffer,sizeof(buffer));
							}
						}
						//Close(connfd);
						read(serverfd,&choice,sizeof(choice));
						Close(serverfd);					
					}break;
				case 9: {// clear cart 
						int ret,numProd,prod;
						char buffer[150];
						float totPrezzo;

						// connection to server M
						serverfd = Socket(AF_INET,SOCK_STREAM,0);
						serverM.sin_addr.s_addr = inet_addr("127.0.0.1");
						serverM.sin_port= htons(1024);
						serverM.sin_family= AF_INET;
						
						Connect(serverfd,(struct sockaddr*) &serverM,sizeof(serverM));
						printf("Connected to server master\n");
						
						// write to serverM
						FullWrite(serverfd,&choice,sizeof(choice));
						FullWrite(serverfd,&user,sizeof(user));
						
						// read from serverM
						FullRead(serverfd,&numProd,sizeof(numProd));
						// write to serverM
						FullWrite(connfd,&numProd,sizeof(numProd));

						for(i = 0; i < numProd; i++){
							FullRead(serverfd,buffer,sizeof(buffer));
							FullWrite(connfd,buffer,sizeof(buffer));
						}
						//Close(connfd);
						read(serverfd,&choice,sizeof(choice));
						Close(serverfd);
					}break;
			}
			// end fork
			exit(0);		
		}
		// closing 
		Close(connfd);
	}
	return 0;
}

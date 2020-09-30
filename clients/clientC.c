// this is client N
// Authentication and store owner operations

#include "lib.h"
int main(){
	// utility
	int sockfd;
	struct sockaddr_in server;
	int choice,x,ret;
	User user;
	x =0;

	
	//--------------------------------------------------------
		// AUTHENTICATION
	//--------------------------------------------------------	
	if(x ==0){
	for(;;){		
			
	       	// set server C options
	       	sockfd = Socket(AF_INET,SOCK_STREAM,0);
		
       		 // set ip,port and domain of socket
       		 server.sin_addr.s_addr = inet_addr("127.0.0.1");
       		 server.sin_port= htons(1025);
       		 server.sin_family= AF_INET;
	
      		 Connect(sockfd,(struct sockaddr*)&server,sizeof(server));

		// connect to server C for using user operations 
		//Connect(sockfd,(struct sockaddr*)&server,sizeof(server));
		sleep(1);

		/*---------------------------
		 * --authenitication
		 *  ------------------------*/
			printf("\nlogin\n");
			printf("--------------------------\n");
			printf("1.login\n2.register\n");
			printf("--------------------------\n");
		
			scanf("%d",&choice);
			userTyping(&user);
			user.type = 1;
			FullWrite(sockfd,&choice,sizeof(choice));
			FullWrite(sockfd,&user,sizeof(user));

			switch(choice){
				case 1:{ // login
					// returning response
					FullRead(sockfd,&ret,sizeof(ret));
					if(ret == 0){
						printf("logged in\n");
						x = 1;
					}else
						printf("wrong password/username\n");
					sleep(1);
				}break;
				case 2:{ // register
				       	// returning response
				      	FullRead(sockfd,&ret,sizeof(ret));
					if(ret == 0){
						printf("register completed\n");
						x == 1;
					}else
						printf("can't register\n");
				       sleep(1);
			       }break;
				default:{
				       	printf("you hit a wrong option\n");
					Close(sockfd);
					}
		} // end switch
			Close(sockfd);
			break;
	} // end for
	} // end if	



	//--------------------------------------------------------
		// CHATTING WITH SERVER N
	//--------------------------------------------------------	
	if(x ==1){
	for(;;){
		// set server N communication
		sockfd = Socket(AF_INET,SOCK_STREAM,0);
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
		server.sin_port= htons(1025);
		server.sin_family= AF_INET;
		
		// connect to server
		Connect(sockfd,(struct sockaddr*)&server,sizeof(server));
		sleep(1);
		system("clear");
		printf("Welcome to VIRTUAL-shop\n");
		printf("-----------------------------------------\n");
		printf("[3]. add store\n[4]. delete store\n[5]. add product\n[6]. delete product\n[7]. exit\n");
		printf("*----------------------------------------\n");
		printf("\n\n");
		scanf("%d",&choice);
		
		// write to server N
		FullWrite(sockfd,&choice,sizeof(choice));
		FullWrite(sockfd,&user,sizeof(user));
		
		// all client N options 
		switch(choice){
			case 3:{ // add store
					char input[50];
					int ret;
					Store store;
					// store name
					printf("insert store name : \n");
					fflush(stdin);
					scanf("%s",store.nomeMag);
					// user owner
					strcpy(store.name,user.name);
					// city 
					printf("\ninsert city : ");
					fflush(stdin);
					scanf("%s",store.city);
					FullWrite(sockfd,&store,sizeof(store));
					FullRead(sockfd,&ret,sizeof(ret));
					if(ret != 0)
						printf("Store exists already \n");
					else
						printf("\nStore added successfully\n");
					sleep(1);
			       }	
				break;
			case 4:{ // delete store
					char input[50];
					int ret;
					Store store;
					printf("\ninsert store name: ");
					fflush(stdin);
					scanf("%s",store.nomeMag);
					FullWrite(sockfd,&store,sizeof(store));
					FullRead(sockfd,&ret,sizeof(ret));
					if(ret == 1)
						printf("\nstore deleted\n");
						
					else if(ret == 0)
						printf("You can't delete this store ! You are not the owner\n");
						else
							printf("delete store error\n");
					sleep(1);
			       }
				break;
			case 5:{ // insert product
					char input[50];
					int ret;
					Product prod;
					printf("insert store name: \n");
					fflush(stdin);
					scanf("%s",prod.nomeMag);
					printf("insert product name :\n");
					fflush(stdin);
					scanf("%s",prod.nomeArticolo);
					printf("insert price : \n");
					fflush(stdin);
					scanf("%f",&prod.prezzo);
					FullWrite(sockfd,&prod,sizeof(prod));
					FullRead (sockfd,&ret,sizeof(ret));
					if(ret == 0)
						printf("Product added successfully\n");
					else if(ret == 2)
						printf("You can add product only if you are the owner\n");
						else
							printf("product add error\n");
					sleep(1);
			       }
				break;
			case 6:{ // delete product
					char input[50];
					int ret;
					Product prod;
					printf("\ninsert store name: ");
					fflush(stdin);
					scanf("%s",prod.nomeMag);
					printf("\ninsert product name: ");
					fflush(stdin);
					scanf("%s",prod.nomeArticolo);
					FullWrite(sockfd,&prod,sizeof(prod));
					FullRead (sockfd,&ret,sizeof(ret));
					if(ret == 0)
						printf("product successfully deleted\n");
					else if(ret == 2)
						printf("\nYou can only delete product of your store\n");
						else
							printf("delete product error\n");
					sleep(1);   
			       }
				break;
			case 7:{
					printf("\ngetting out !\n\n");
					fflush(stdout);
	
					sleep(1);
					Close(sockfd);
					return 0;
				}
				break;
			default:{
					printf("\ntry again.\n");
					fflush(stdout);
					sleep(1);
                                        read(sockfd,&choice,sizeof(choice));
                                        close(sockfd);
				}break;
		} // end switch
	} // end if
	} // end for
}

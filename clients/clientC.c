// this is client C 
// Authentication and doing user like operations

#include "lib.h"
int main(){
	//utility 
//	struct termios oflags, nflags;
	int sockfd;
	struct sockaddr_in servaddr;
	int x = 0;
	int choice,ret,i;
	User user;

	//--------------------------------------------------------
		// AUTHENTICATION
	//--------------------------------------------------------	
	if(x ==0){
	for(;;){		
		 // set server C options
	       	 sockfd = Socket(AF_INET,SOCK_STREAM,0);

       		 // set ip,port and domain of socket
       		 servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
       		 servaddr.sin_port= htons(1026);
       		 servaddr.sin_family= AF_INET;
	
      		 Connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

		// connect to server C for using user operations 
		//Connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
		sleep(1);

		/*---------------------------
		 * --authenitication
		 *  ------------------------*/
			printf("\nlogin\n");
			printf("--------------------------\n");
			printf("1. login\n2. register\n");
			printf("--------------------------\n");
		
			scanf("%d",&choice);
			userTyping(&user);
			user.type = 0;
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
				       	read(sockfd,&choice,sizeof(choice));
					Close(sockfd);
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


	if(x ==1){
	for(;;){
		 // set server C options
		 sockfd = Socket(AF_INET,SOCK_STREAM,0);

       		 // set ip,port and domain of socket
		 servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
       		 servaddr.sin_port= htons(1026);
       		 servaddr.sin_family= AF_INET;

       		 Connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
		// connection to server C		
                //Connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));


		sleep(1);

		/* ---------------------------------
		 * --switch case for user operations 	
		 * ---------------------------------*/
		
		system("clear");
		printf("Welcome to VIRTUAL-shop\n");
		printf("--------------------------------------------------\n");
		printf("[7]. add product\n[8]. show cart\n[9]. clear cart\n[10]. exit\n");
		printf("*-------------------------------------------------\n");
		printf("\n\n");
		scanf("%d",&choice);
		
		// chat with server C
		FullWrite(sockfd,&choice,sizeof(choice));
		FullWrite(sockfd,&user,sizeof(user));
		
		switch(choice){
			case 7:{ // add prod to the cart
					char input[150];
					int ret,numProducts;
					// read from server number of products found
					FullRead(sockfd,&numProducts,sizeof(numProducts));
					printf("found : %d products \n",numProducts);
					if(numProducts > 0){
						for(i=0;i<numProducts;i++){
							FullRead(sockfd,&input,sizeof(input));
							printf("[%2d] --- %s",i+1,input);
						}
						int prod;
						printf("\nInsert the number of what product you want : ");
						fflush(stdin);
						scanf("%d",&prod);
						FullWrite(sockfd,&prod,sizeof(prod));
						FullRead(sockfd,&ret,sizeof(ret));
						if(ret != 0)
								printf("\nThis product is not avaible\n");
						else
								printf("\nProduct correctly added in the cart\n");
						sleep(1);
					}else{
						FullRead(sockfd,&input,sizeof(input));
						int prod;
						printf("\ninsert a number to get out : \n");
						fflush(stdin);
						scanf("%d",&prod);
						FullWrite(sockfd,&prod,sizeof(prod));
						FullRead(sockfd,&ret,sizeof(ret));
						if(ret == 0)
								printf("\nGetting outn");
						sleep(1);
					}

			       }break;
			case 8:{ // show cart
					char input[150];
					int ret,numProducts;
					FullRead(sockfd,&numProducts,sizeof(numProducts));
					if(numProducts > 0){
						for(i = 0 ; i < numProducts; i++){
							FullRead(sockfd,&input,sizeof(input));
							printf("[%2d] --- %s",i+1,input);
						}
						// press a number to go back
						i = 999;
						printf("press a number to go back :");
						scanf("%d",&i);
						if(i != 999)
							sleep(1);
						
					}else{
						printf("\nCart is empty\n");
						sleep(1);
					}
				}break;
			case 9: {// clear cart 
					char input[150];
					int ret,numProducts;
					FullRead(sockfd,&numProducts,sizeof(numProducts));
					for(i = 0 ; i < numProducts; i++){
						FullRead(sockfd,&input,sizeof(input));
						printf("[%2d] --- %s",i+1,input);
					}
					fflush(stdout);
					sleep(1);
				}break;
			case 10: {// closing 
					printf("\nGetting out\n");
					fflush(stdout);
					sleep(1);
					read(sockfd,&choice,sizeof(choice));
					Close(sockfd);
					return 0;
				}break;
			default:{
					printf("try again\n");
					fflush(stdout);
					read(sockfd,&choice,sizeof(choice));
					Close(sockfd);
				}break;
		}// end switch
	} //end fo
	} // end if
}


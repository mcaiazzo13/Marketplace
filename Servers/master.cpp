// this is server Master
// read messages from server C/N and then do the actual operation

#include "lib.h"
int main(){
	// utility 
	int connfd,listenfd;
	int enable = 1;
	struct sockaddr_in server,client;
	Store store;
	Product prod;
	User user;
	
	// listen socket
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	Setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&enable,sizeof(enable));
	
	// set server M
	server.sin_family = AF_INET;
	server.sin_port = htons(1024);
	server.sin_addr.s_addr= htonl(INADDR_ANY);
	
	// bind
	Bind(listenfd,(struct sockaddr*)&server,sizeof(server));
	
	// listening 
	Listen(listenfd,1024);
	int choice;
	for(; ;){
		int sizeC = sizeof(client);
		int i,ret;
		
		// accept connection 
		connfd = Accept(listenfd,(struct sockaddr*)&client,&sizeC);
		
		// create a child process to handle communnication
		if(!Fork()){
			// close listen becasuse i need connfd
			close(listenfd);
			
			// reading what serverN/C wrote
			FullRead(connfd,&choice,sizeof(choice));
			
			switch(choice){
				case 1:{ // login
					       FullRead(connfd,&user,sizeof(user));
					       ret = Login(&user);
					       FullWrite(connfd,&ret,sizeof(ret));

					       Close(connfd);
					
				       }break;
				case 2:{ // register user
					       FullRead(connfd,&user,sizeof(user));
					       ret = newUser(&user);
					       FullWrite(connfd,&ret,sizeof(ret));

					       Close(connfd);
				       }break;
				case 3: {// add store
						FullRead(connfd,&store,sizeof(store));
						ret = addStore(&store);
						FullWrite(connfd,&ret,sizeof(ret));
						
						Close(connfd);
					}break;
				case 4: {// delete store
						FullRead(connfd,&store,sizeof(store));
						FullRead(connfd,&user,sizeof(user));
						ret = deleteStore(&store,&user);
						FullWrite(connfd,&ret,sizeof(ret));
						
						Close(connfd);
					}break;
				case 5: {// add product 
						FullRead(connfd,&prod,sizeof(prod));
						FullRead(connfd,&user,sizeof(user));
						if(checkProduct(&prod,&user) == 0)
							ret = addProduct(&prod);
						else
							ret = 2;
						FullWrite(connfd,&ret,sizeof(ret));
						
						Close(connfd);
					}break;
				case 6: {//delete product
						FullRead(connfd,&prod,sizeof(prod));
						FullRead(connfd,&user,sizeof(user));
						if(checkProduct(&prod,&user) == 0)
							ret = deleteProduct(&prod);
						else
							ret = 2;
						FullWrite(connfd,&ret,sizeof(prod));
						
						Close(connfd);
					}break;
				case 7: {// add product to the list
						// show products to client
						int numProd,app,prodScelto;
						char buffer[150] ;
						strcpy(buffer,"\nNo product avaible\n");
						ret = 0;
						FullRead(connfd,&user,sizeof(user));
						numProd = getProduct();
						if(numProd > 0){
							FullWrite(connfd,&numProd,sizeof(numProd));
							FILE *in;
							in = fopen("./PRODOTTI","r");
							if(in != NULL){
								fscanf(in,"%d",&app);
								fseek(in,1,SEEK_CUR);
								for(i = 0; i < numProd; i++){
									fgets(buffer,150,in);
									FullWrite(connfd,&buffer,sizeof(buffer));
								}
							}
							// client choose product and add to cart
							FullRead(connfd,&prodScelto,sizeof(prodScelto));
							ret = addCart(&prodScelto,&user);
							FullWrite(connfd,&ret,sizeof(ret));
							fclose(in);
							
							Close(connfd);
						}else{
							FullWrite(connfd,&numProd,sizeof(numProd));
							FullWrite(connfd,&buffer,sizeof(buffer));
							FullRead(connfd,&prodScelto,sizeof(prodScelto));
							FullWrite(connfd,&ret,sizeof(ret));
							Close(connfd);
						}
					}break;
				case 8: {// show cart
						int numProd,app,prodScelto;
						char buffer[150],nomeCarrello[50];
						FullRead(connfd,&user,sizeof(user));
						// get number of product inside cart
						numProd = getProductCart(&user);
						FullWrite(connfd,&numProd,sizeof(numProd));
						FILE *in;
						strcpy(nomeCarrello,"./cart_");
						strcat(nomeCarrello,strupr(user.name));
						in = fopen(nomeCarrello,"r");
						if(in != NULL){
							fscanf(in,"%d",&app);
							fseek(in,1,SEEK_CUR);
							for(i = 0; i < numProd; i++){
								fgets(buffer,150,in);
								FullWrite(connfd,&buffer,sizeof(buffer));
							}
							fclose(in);
						}else
							fclose(in);
						Close(connfd);
					}break;	
				case 9: {// clear cart
						int numProd,app,prodScelto;
						char buffer[150],nomeCarrello[50];
						char app1[50],app2[50],stringprice[10];
						float price,totprice = 0;
						FullRead(connfd,&user,sizeof(user));
						numProd = getProductCart(&user);
						FullWrite(connfd,&numProd,sizeof(numProd));
						FILE *in,*out;
						strcpy(nomeCarrello,"./cart_");
						strcat(nomeCarrello,strupr(user.name));
						in = fopen(nomeCarrello,"r");
						if(in != NULL){
							fscanf(in,"%d",&app);
							fseek(in,1,SEEK_CUR);
							for(i = 0; i < numProd; i++){
								fscanf(in,"%s%s%s",app1,app2,stringprice);
								strcpy(buffer,app1);
								strcat(buffer," ");
								strcat(buffer,app2);
								strcat(buffer," ");
								strcat(buffer,stringprice);
								strcat(buffer,"\n");
								FullWrite(connfd,&buffer,sizeof(buffer));
							}
							FullWrite(connfd,&totprice,sizeof(totprice));
							out = fopen("./NEW","w");
							fprintf(out,"%d\n",atoi("0"));
							fclose(in);
							fclose(out);
							remove(nomeCarrello);
							rename("./NEW",nomeCarrello);
						}
						Close(connfd);
					}break;
			}// end switch
			// end fork
			exit(0);
		}
		// close connection
		Close(connfd);
	}
	return 0;
}

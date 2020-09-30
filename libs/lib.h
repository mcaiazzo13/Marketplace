//  this library contains all kind of functions that allow the project works.

#include "wrapper.h" 
#include <termios.h>

/*------------------------
 -- STRUCT
 -------------------------*/

typedef struct Store{
	char nomeMag[30];
	char name[30];
	char city[50];
} Store;

typedef struct Product{
	char nomeMag[30];
	char nomeArticolo[50];
	float prezzo;
} Product;

typedef struct User{
	char name[30];
	char pass[10];
	int type; // 0 user C | 1 user N
} User;

/*---------------------------
  --FUNCTIONS 
  --------------------------*/

//  utility options
ssize_t FullWrite(int ,const void *,size_t);
ssize_t FullRead(int ,void *,size_t);
User* userTyping(User*);
char *strupr(char * );
int lookupStore(Store *);
int lookupProduct(int *,Product *);
int checkProduct(Product*,User*);
int getProduct();

// shop options
int addStore(Store *);
int deleteStore(Store*,User *);

//  product options
int addProduct(Product*);
int deleteProduct(Product*);

//  user option
int Login(User *);
int newUser(User *);
int addCart(int *,User*);
int getProductCart(User*);


/*---------------------------------------
  --IMPLEMENTS
 * --------------------------------------*/


//  fullwrite
ssize_t FullWrite(int fd,const void *buff,size_t size){
	size_t nleft = size;
	ssize_t nwrite;
	while(nleft>0){
		if((nwrite = write(fd,buff,size))<0)
			if(errno == EINTR)
				continue;
			else
				exit(nwrite);
		nleft -= nwrite;
		buff += nwrite;
	}
	return(nleft);
}


//  fullread
ssize_t FullRead(int fd,void *buff,size_t size){
	size_t nleft = size;
	ssize_t nread;
	while(nleft>0){
		if((nread = read(fd,buff,size)) < 0)
			if(errno == EINTR)
				continue;
			else
				exit(nread);
		else if(nread == 0)
			break;
		nleft -= nread;
		buff += nread;
	}
	buff = 0;
	return(nleft);
}

//  lowercase to UPPERCASE
char *strupr(char* string){
	int i = 0;
	int len = strlen(string);
 	char *app;
	app =(char*)calloc(len,sizeof(char));
	while(string[i] != '\0'){
    		if(string[i] >= 'a' && string[i] <= 'z')
        		app[i] = string[i] ^ 32;
			else
			app[i] = string[i];
      	i++;
	}
	return app;
}

//  user can write username and password (hidden)
User* userTyping(User* user){
        struct termios oflags,nflags;
        printf("Username: "); scanf("%s",user->name);

        //  shut down echo
        tcgetattr(fileno(stdin),&oflags);
        nflags = oflags;
        nflags.c_lflag &= ~ECHO;
        nflags.c_lflag |= ECHONL;

        if(tcsetattr(fileno(stdin),TCSANOW,&nflags) != 0) {
                 perror("tcsetattr");
                return NULL;
        }

        printf("Password: "); scanf("%s",user->pass);

        //  restore echo

        if(tcsetattr(fileno(stdin),TCSANOW,&oflags) != 0){
                perror("tcsetattr\n");
                exit(1);
        }
        return user;
}

// when clientN wants to add a new store. If that store already exists it returns 0 else 2
int lookupStore(Store *mag){
	FILE *db;
	char riga[150];
	char nome[30];
	int numMag,i;
	db = fopen("./DB","r");
	if(db != NULL){
		fscanf(db,"%d",&numMag);
		fseek(db,1,SEEK_CUR);
		for(i = 0;i<numMag;i++){
			fscanf(db,"%s",nome);
			fgets(riga,150,db);
			if(strcmp(nome ,strupr(mag->nomeMag)) == 0)
					return 0; // store exists
		}				
		fclose(db);
		return 2; // store doesn't exist
	}else{
		perror("error opening file\n");
		exit(1);
	}
}


// Quando viene effettuata la procedura di inserimento di un prodotto all'interno del
// carrello utente questa procedura permette,a partire dal numero associato all'elenco dei
// prodotti visualizzati a schermo,di selezionare il prodotto scelto scorrendo le righe
// fino al raggiungimento dell'articolo in questione 
int lookupProduct(int *sceltaArt,Product *art){
	FILE *prodotti;
	Product app;
	char riga[150];
	prodotti = fopen("./PRODOTTI","r");
	int num_art,i;
	if(prodotti != NULL){
		fscanf(prodotti,"%d",&num_art);
		fseek(prodotti,1,SEEK_CUR);
		if(*sceltaArt <= num_art){
			for(i = 0;i<*sceltaArt;i++){
				fscanf(prodotti,"%s%s%f",app.nomeMag,app.nomeArticolo,&app.prezzo);
				strcpy(art->nomeMag,app.nomeMag);
				strcpy(art->nomeArticolo,app.nomeArticolo);
				art->prezzo = app.prezzo;
			}				
			fclose(prodotti);
			return 0;
		}else{
			fclose(prodotti);
			return 1;
		}
	}else{
		perror("error opening file\n");
		exit(1);
	}
}

// add new store in file DB
int addStore(Store *mag){	
	FILE *db_inizio;
	FILE *db_fine;
	char buffer[150] = {0};
	int numNegozi,i;
	if(lookupStore(mag) == 2){
		db_inizio = fopen("./DB","r");
		db_fine = fopen("./NewDB","w");
		if(db_inizio != NULL && db_fine != NULL){
			fscanf(db_inizio,"%d",&numNegozi);
			numNegozi++;
			fprintf(db_fine,"%d\n",numNegozi);
			fseek(db_inizio,1,SEEK_CUR);
			for(i = 0; i < numNegozi-1; i++){
				fgets(buffer,150,db_inizio);
				fprintf(db_fine,"%s",buffer);
			}
			fprintf(db_fine,"%s %s %s\n",strupr(mag->nomeMag),strupr(mag->name),strupr(mag->city));
			fclose(db_inizio);
			fclose(db_fine);
			remove("./DB");
			rename("./NewDB","./DB");
			return 0; // store added
		}else{
			perror("error opening File");
			exit(1);
		}
	}else 
		return 1;
}


// delete store (only the owner is allowed)
int deleteStore(Store *mag,User *user){
	printf("\ndeleting store\n");
	
	FILE *db_inizio;
	FILE *db_fine;
	FILE *db_num;
	char nome[30] = {0}; // store name
	char prop[30] = {0}; // owner name
	char buffer[150] = {0};
	int numNegozi,i,ret,new_num;
	
	numNegozi=0;
	new_num = 0;

	if(lookupStore(mag) == 0){
		db_inizio = fopen("./DB","r");
		db_fine = fopen("./NewDB","w");
		db_num = fopen("./DEF","w");
		if(db_inizio != NULL && db_fine != NULL && db_num != NULL){
			fscanf(db_inizio,"%d",&numNegozi);
			fseek(db_inizio,1,SEEK_CUR);
			for(i = 0; i < numNegozi; i++){
				fscanf(db_inizio,"%s%s",nome,prop);
				fgets(buffer,150,db_inizio);
				if(strcmp(strupr(nome),strupr(mag->nomeMag)) != 0 || strcmp(strupr(prop),strupr(user->name)) != 0){
					strcat(nome," ");
					strcat(nome,prop);
					strcat(nome,buffer);
					fprintf(db_fine,"%s",strupr(nome));
					new_num++;
				}else
					ret = 1;
			}
			fclose(db_inizio);
			fclose(db_fine);
			db_fine = fopen("./NewDB","r");
			fprintf(db_num,"%d\n",new_num);
			
			for(i = 0;i<new_num;i++){
				fgets(buffer,150,db_fine);
				fprintf(db_num,"%s",buffer);
			}
			fclose(db_fine);
			fclose(db_num);
			remove("./DB");
			remove("./NewDB");
			rename("./DEF","./DB");
			// delete cascade
			if(ret == 1){
				int numProd;
				db_inizio = fopen("./PRODOTTI","r");
				db_fine = fopen("./NEWP","w");
				db_num = fopen("./DEF","w");
				int new_num_prod = 0;
				char nomeMag[30];
				if(db_inizio != NULL && db_fine != NULL && db_num != NULL){
					fscanf(db_inizio,"%d",&numProd);
					for(i = 0; i < numProd; i++){
						fscanf(db_inizio,"%s",nomeMag);
						fgets(buffer,150,db_inizio);
						if(strcmp(nomeMag,strupr(mag->nomeMag)) != 0 ){
							strcat(nomeMag,buffer);
							fprintf(db_fine,"%s",strupr(nomeMag));
							new_num_prod++;
						}	
					}
					fclose(db_inizio);
					fclose(db_fine);
					db_fine = fopen("./NEWP","r");
					fprintf(db_num,"%d\n",new_num_prod);
					for(i = 0;i<new_num_prod;i++){
						fgets(buffer,150,db_fine);
						fprintf(db_num,"%s",buffer );
					}
					fclose(db_fine);
					fclose(db_num);
					remove("./PRODOTTI");
					remove("./NEWP");
					rename("./DEF","./PRODOTTI");
				}
			}
			return ret;
		}else{
			perror("error opening File");
			exit(1);	
		}
	}else
			return 1;
}
// add product to store
int addProduct(Product *prod){
	FILE *db_inizio;
	FILE *db_fine;
	char app[150] = {0};
	int i,numProd;
	db_inizio = fopen("./PRODOTTI","r");
	db_fine = fopen("./newP","w");
	if(db_inizio != NULL && db_fine != NULL){
		fscanf(db_inizio,"%d",&numProd);
		numProd++;
		fprintf(db_fine,"%d\n",numProd);
		fseek(db_inizio,1,SEEK_CUR);
		for(i = 0;i<numProd-1;i++){
			fgets(app,150,db_inizio);
			fprintf(db_fine,"%s",app);
		}
		fprintf(db_fine,"%s %s %5.2f \n",strupr(prod->nomeMag),strupr(prod->nomeArticolo),prod->prezzo);
		fclose(db_inizio);
		fclose(db_fine);
		remove("./PRODOTTI");
		rename("./newP","./PRODOTTI");	
		return 0;
	}else{
		perror("error opening File");
		exit(1);
	}
}
// Questa funzione permette di eliminare un prodotto all'interno di un Magazzino
// prima di questa operazione si è effettuato un controllo per verificare che l'inserimento
// fosse effettuato dal proprietario del magazzino dove si vuole inserire il prodotto
// tramite la funzione checkProduct(...);
int deleteProduct(Product* prod){
	FILE *DB_inizio;
	FILE *DB_fine;
	FILE *DB_num;
	FILE* my;
	int numProd,i,new_num;
	char buffer[150],mag[30],prodotto[50];

	numProd = 0;
	new_num = 0;

	DB_inizio = fopen("./PRODOTTI","r");
	DB_fine = fopen("./NEWP","w");
	DB_num = fopen("./DEF","w");
	if(DB_inizio != NULL && DB_fine != NULL && DB_num != NULL){
		fscanf(DB_inizio,"%d",&numProd);
		fseek(DB_inizio,1,SEEK_CUR);
		for(i = 0; i < numProd; i++){
			fscanf(DB_inizio,"%s%s",mag,prodotto);
			fgets(buffer,150,DB_inizio);
			if(strcmp(prodotto,strupr(prod->nomeArticolo)) != 0 || strcmp(mag,strupr(prod->nomeMag)) != 0){
				strcat(mag," ");
				strcat(mag,prodotto);
				strcat(mag,buffer);
				fprintf(DB_fine,"%s",strupr(mag));
				new_num++;
			}	
		}
		fclose(DB_inizio);
		fclose(DB_fine);
		DB_fine = fopen("./NEWP","r");
		fprintf(DB_num,"%d\n",new_num);
		for(i = 0;i<new_num; i++){
			fgets(buffer,150,DB_fine);
			fprintf(DB_num,"%s",buffer);
	//		fprintf(DB_num,"%s","ciao");
		}
		fclose(DB_fine);
		fclose(DB_num);
		remove("./PRODOTTI");
		remove("./NEWP");
		rename("./DEF","./PRODOTTI");
		return 0;
	}else{
		perror("error opening File");
		exit(1);
	}
}

// Login
int Login(User *user){
	FILE *in;
	in = fopen("./passwd","r");
	int numUser,i;
	char nome[30] = {0};
	char pass[10] = {0};
	if(in != NULL){
		fscanf(in,"%d",&numUser);
		fseek(in,1,SEEK_CUR);
		for(i = 0;i<numUser;i++){
			fscanf(in,"%s%s",nome,pass);
			if(strcmp(strupr(nome),strupr(user->name)) == 0)
				if(strcmp(pass,user->pass) == 0){	
					fclose(in);	
					return 0;
				}else{
					fclose(in);
					return 1;
				}
		}
		fclose(in);
		return 2;
	}else{
		perror("error opening File");
		exit(1);
	}
	
}
// create new user. If user C create cart_$USER file 
int newUser(User *user){
	if(strlen(user->name) > 30 )
		exit(1);
	if(strlen(user->pass) > 10)
		exit(2);
	FILE *in,*out;
	char buffer [150] = {0};
	int numUser,i;
	in = fopen("./passwd","r");
	out = fopen("./NEW","w");
	if(in != NULL && out != NULL){
		if(Login(user) == 2){
			fscanf(in,"%d",&numUser);
			fseek(in,1,SEEK_CUR);
			numUser++;
			fprintf(out,"%d\n",numUser);
			for(i = 0; i < numUser -1 ; i++){
				fgets(buffer,150,in);
				fprintf(out,"%s",buffer);		
			}
			fprintf(out,"%s %s\n",user->name,user->pass);
			// create cart file
			if(user->type == 0){
				FILE *carrello;
				char nome_carr[50];
				strcpy(nome_carr,"./cart_");
				strcat(nome_carr,strupr(user->name));
				carrello = fopen(nome_carr,"w");
				fprintf(carrello,"%d\n",atoi("0"));
				fclose(carrello);
			}
			fclose(in);
			fclose(out);
			remove("./passwd");
			rename("./NEW","./passwd");
			return 0;
		}
	}else{
		perror("error opening File");
		exit(3);
	}
}

// add product to cart (file cart_$USER) 
int addCart(int *sceltaArt,User *user){
	FILE *carrello,*out;
	Product uscita;
	char nomeCarr[50];
	char buffer[150] = {0};
	int numOgg,i;
	strcpy(nomeCarr,"./cart_");
	strcat(nomeCarr,strupr(user->name));
	
	// open file cart_$USER
	carrello = fopen(nomeCarr,"r");
	out = fopen("./NEW","w");
	if(carrello == NULL){
		carrello = fopen(nomeCarr,"w");
		fprintf(carrello,"%d\n",atoi("0"));
		fclose(carrello);
		carrello = fopen(nomeCarr,"r");
	} 
	if(carrello != NULL && out != NULL){
		lookupProduct(sceltaArt,&uscita);
		fscanf(carrello,"%d",&numOgg);
		fseek(carrello,1,SEEK_CUR);
		numOgg++;
		fprintf(out,"%d\n",numOgg);
		for(i=0;i<numOgg-1;i++){
			fgets(buffer,150,carrello);
			fprintf(out,"%s",buffer);
		}
		fprintf(out,"%s %s %5.2f\n",uscita.nomeMag,uscita.nomeArticolo,uscita.prezzo);
		fclose(carrello);
		fclose(out);
		remove(nomeCarr);
		rename("./NEW",nomeCarr);
		return 0;
	}else{
		perror("\nerror opening FILE\n");
		exit(1);
	}
}

// return num of product in file PRODOTTI
int getProduct(){
	FILE *in;
	int num = 0;
	in = fopen("./PRODOTTI","r");
	if(in != NULL)
		fscanf(in,"%d",&num);
	else
		perror("\nerror open file");
	fclose(in);
	return num;
}

// return product in cart_$USER
int getProductCart(User *user){
	FILE *in;
	char nomeCarr[50];
	strcpy(nomeCarr,"./cart_");
	strcat(nomeCarr,strupr(user->name));
	in = fopen(nomeCarr,"r");
	if(in != NULL){
		int numArt;
		fscanf(in,"%d",&numArt);
		fclose(in);
		return numArt;
	}else
		return 0;
}

// check if the product is added by the store owner
int checkProduct(Product *prod,User *user){
	char name[30] = {0};
	char nomeMag[30] = {0};
	char buffer[150];
	FILE *mag;
	int dimMag,i;
	mag = fopen("./DB","r");
	if(mag != NULL){
		fscanf(mag,"%d",&dimMag);
		fseek(mag,1,SEEK_CUR);
		for(i = 0;i<dimMag;i++){
			fscanf(mag,"%s",nomeMag);
			if(strcmp(strupr(prod->nomeMag),nomeMag) == 0){
				fscanf(mag,"%s",name);
				i = dimMag;
			}
			fgets (buffer,150 ,mag);
		}
		fclose(mag);
		if(strcmp(strupr(user->name),name) == 0) 
			return 0;
		else 
			return 1;
	}else{
		perror("\nerror open file");
		exit(1);
	}
}


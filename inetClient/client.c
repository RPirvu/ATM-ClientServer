#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<stdbool.h>
#include <string.h>
#include<fcntl.h>
#include <pthread.h>

#define BUFFSIZE 1024
char buffer[BUFFSIZE];


typedef struct admin{
    int adminid;
    char username[25];
    char password[20];
}admin;

typedef struct user{
    int userid;
    char username[25];
    char password[20];
    int account_num;
    float balance;
    bool is_deleted;
}user;

void user_operation(int serverSide,int type,struct sockaddr_in serv;);

void add_new_user(int serverSide,int type, struct sockaddr_in serv){
    int status=0;
    user new_user,new_user1;

    // Input Utilizator
    write(1,"Introduceti Nume de Utilizator : ",sizeof("Introduceti Nume de Utilizator : "));
	scanf(" %[^\n]",new_user.username);
    write(1,"Introduceti Parola : ",sizeof("Introduceti Parola : "));
	scanf(" %[^\n]",new_user.password);

    // Trimite utilizatorul nou
    write(serverSide,&new_user,sizeof(user));
    read(serverSide,&new_user1,sizeof(user));

    // Citeste statusul primit de la server
    read(serverSide,&status,sizeof(int));

    if(status){ 
        write(1,"Id-ul contului este : ",sizeof("Id-ul contului este : "));
        printf("%d\n",new_user1.userid);
    }
    else write(1,"Eroare la crearea utilizatorului\n",sizeof("Eroare la crearea utilizatorului\n"));
    printf("\n");
    printf("\n");
    user_operation(serverSide,type,serv);
}

void add_admin(int serverSide,int type, struct sockaddr_in serv){
    int status=0;
    admin new_user,new_user1;

    // Input Utilizator
    write(1,"Introduceti Nume de Utilizator : ",sizeof("Introduceti Nume de Utilizator : "));
	scanf(" %[^\n]",new_user.username);
    write(1,"Introduceti Parola : ",sizeof("Introduceti Parola : "));
	scanf(" %[^\n]",new_user.password);

     // Trimite utilizatorul nou
    write(serverSide,&new_user,sizeof(admin));
    read(serverSide,&new_user1,sizeof(admin));

    // Citeste statusul primit de la server
    read(serverSide,&status,sizeof(int));
    if(status){
        write(1,"Id-ul contului este : ",sizeof("Id-ul contului este : "));
        printf("%d\n",new_user1.adminid);
    }
    else write(1,"Eroare la crearea utilizatorului\n",sizeof("Eroare la crearea utilizatorului\n"));
    printf("\n");
    printf("\n");

    user_operation(serverSide,type,serv);
}

int user_login(int serverSide){
    user new_user;
    int status;

    // Input de la utilizator
    write(1,"Id : ",sizeof("Id : "));
    scanf("%d",&new_user.userid);
    write(1,"Parola : ",sizeof("Parola : "));
    scanf("%s",new_user.password);

    // Trimite inputul catre server
    write(serverSide,&new_user.userid,sizeof(new_user.userid));
    write(serverSide,&new_user.password,sizeof(new_user.password));

    // Primeste statusul de la server
    read(serverSide,&status,sizeof(int));
    return status;
}

void login(int serverSide,int arr[]){
    int type = 2,status;


    // write(1,"Autentificare\nAlegeti Tipul de Utilizator\n",sizeof("Autentificare\nAlegeti Tipul de Utilizator\n"));
    // write(1,"1 : Admin\n",sizeof("1 : Admin\n"));
    // write(1,"2 : Utilizator Normal\n",sizeof("2 : Utilizator Normal\n"));

    // // Input tipul de utilizator
    // scanf("%d",&type);

    switch (type){
    case 1 : write(serverSide,&type,sizeof(type)); // Trimite tipul utilizatorului catre server
        write(1,"Utilizator nedefinit\n",sizeof("Utilizator nedefinit\n"));
        break;
    case 2 : write(serverSide,&type,sizeof(type)); // Trimite tipul utilizatorului catre server
        status = user_login(serverSide);
        break;
    default: write(1,"Utilizator nedefinit\n",sizeof("Utilizator nedefinit\n"));
        status = 2;
        break;
    }

    if(status==1) write(1,"Autentificare cu succes\n",sizeof("Autentificare cu succes\n")); // Afisare Meniu
    else write(1,"Eroare la autentificare\n",sizeof("Eroare la autentificare\n"));

    // Modifica type si status 
    arr[0] = type;
    arr[1] = status;
}


void Exit(int serverSide){
    write(1,"Deconectare...\n",sizeof("Deconectare...\n"));
    close(serverSide);
    exit(0);
}

void user_deposit(int serverSide,int type, struct sockaddr_in serv){
    float amount,balance;
    int status=0;
    // Input Utilizator {Suma - int}
    write(1,"Introduceti suma pentru depozitare\n",sizeof("Introduceti suma pentru depozitare\n"));
    scanf("%f",&amount);
    // Trimite Input catre server
    write(serverSide,&amount,sizeof(float));
    read(serverSide,&balance,sizeof(balance));
    // Citeste status
    read(serverSide,&status,sizeof(int));
    if(status==1){
        write(1,"Tranzactia a fost efectuata cu succes\n",sizeof("Tranzactia a fost efectuata cu succes\n"));
        printf("Sold Curent : %f\n",balance);
    }
    else write(1,"Eroare ~ Tranzactia nu a putut fi efectuata\n",sizeof("Eroare ~ Transactia nu a putut fi efectuata\n"));
     printf("\n");
    printf("\n");
    user_operation(serverSide,type,serv);
}

void user_withdraw(int serverSide,int type, struct sockaddr_in serv){
    float amount,balance;
    int status=0;
    // Input Utilizator {Suma - int}
    write(1,"Introduceti suma pentru retragere\n",sizeof("Introduceti suma pentru retragere\n"));
    scanf("%f",&amount);
    // Trimite Input catre server
    write(serverSide,&amount,sizeof(float));
    read(serverSide,&balance,sizeof(balance));
    // Citeste status
    read(serverSide,&status,sizeof(int));
    if(status==1) 
    {
        write(1,"Tranzactia a fost efectuata cu succes\n",sizeof("Tranzactia a fost efectuata cu succes\n"));
        printf("Sold Curent : %f\n",balance);
    }
    else write(1,"Eroare ~ Tranzactia nu a putut fi efectuata\n",sizeof("Eroare ~ Tranzactia nu a putut fi efectuata\n"));
        printf("\n");
    printf("\n");
    user_operation(serverSide,type,serv);
}

void balance_enquiry(int serverSide,int type, struct sockaddr_in serv){
    float balance;
    int status; 
    // Citeste status si soldul contului
    read(serverSide,&status,sizeof(status));
    read(serverSide,&balance,sizeof(balance));
    if(status==1) printf("Sold Curent : %f\n",balance);
    else write(1,"Eroare\n",sizeof("Eroare\n")); 
        printf("\n");
    printf("\n");
    user_operation(serverSide,type,serv);   
}

void change_user_password(int serverSide,int type, struct sockaddr_in serv){
    char old_pass[20],new_pass[20];
    int status=0;
    // Input utilizator {nume utilizator && parola : String}
    write(1,"Introduceti parola veche\n",sizeof("Introduceti parola veche\n"));
    scanf(" %[^\n]",old_pass);
    write(1,"Introduceti parola noua\n",sizeof("Introduceti parola noua\n"));
    scanf(" %[^\n]",new_pass);
    // Trimite input catre server
    write(serverSide,&old_pass,sizeof(old_pass));
    write(serverSide,&new_pass,sizeof(new_pass));
    // Citeste status
    read(serverSide,&status,sizeof(int));
    if(status) write(1,"Modificarea a fost efectuata\n",sizeof("Modificarea a fost efectuata\n"));
    else write(1,"Eroare ~ Verificati id si parola\n",sizeof("Eroare ~ Verificati id si parola\n"));
        printf("\n");
    printf("\n");
    user_operation(serverSide,type,serv);
}

void view_user_details(int serverSide,int type, struct sockaddr_in serv){
    int status=0;
    // Citeste status 
    read(serverSide,&status,sizeof(status));
    if(status){  // status == 1
        if(type==2){
		    user new_user;
		    read(serverSide,&new_user,sizeof(new_user)); // citeste utilizatorul 
            printf("ID Utilizator = %d\n",new_user.userid);
		    printf("Nume de utilizator = %s\n",new_user.username);
		    printf("Numar cont =%d\n",new_user.account_num);
		    printf("Sold = %f\n",new_user.balance);	    
        }
    }
	else write(1,"Utilizatorul nu a fost gasit\n",sizeof("Utilizatorul nu a fost gasit\\n"));
	    printf("\n");
    printf("\n");
    user_operation(serverSide,type,serv);
}

void checkout(int serverSide, int type, struct sockaddr_in serv){
    int status=0;
    char text[20];
    char filename[50]="Cont";  
    // citeste status 
    read(serverSide,&status,sizeof(status)); 
    if(status){
        if(type==2){
            user new_user;
            // citeste utilizatorul
            read(serverSide,&new_user,sizeof(new_user));
            // concatenare nume fisier
            sprintf(text, "%d", new_user.account_num);
            strcat(filename,text);
            strcat(filename, ".txt");
            // creare si scriere in fisier
            FILE *output_file;
            output_file = fopen(filename, "w");
            fprintf(output_file,"Nume de utilizator: %s \n", new_user.username);
            fprintf(output_file,"Numar cont: %d \n", new_user.account_num);
            fprintf(output_file,"Sold: %f \n", new_user.balance);
            fclose(output_file);
        }
    }
        printf("\n");
    printf("\n");
    user_operation(serverSide,type,serv);
}

void send_report(int serverSide,int type, struct sockaddr_in serv){
     char buffer[256];
     FILE *file;
     int filesize;
     char filename[50];
     write(1,"Nume Fisier\n",sizeof("Nume Fisier\n"));
     scanf("%s",filename);
     file = fopen(filename,"r");
     if (file == NULL) {
     	write(1,"Fisierul nu exista!\n",sizeof("Fisierul nu exista!\n"));
     } else { 
     	write(1,"Fisierul a fost gasit\n",sizeof("Fisierul a fost gasit\n"));
     	fseek(file, 0, SEEK_END);
        filesize = ftell(file);
        rewind(file);
	write(1,"Se incepe transmiterea fisierului\n",sizeof("Se incepe transmiterea fisierului\n"));     	
     }
     
     while (true){
     	int bytes_read = fread(buffer, 1, sizeof(buffer), file);
     	if (bytes_read == 0) // fisierul a fost citit
            break;
            
        if (bytes_read < 0){
            write(1,"Eroare la citirea din fisier\n",sizeof("Eroare la citirea din fisier\n"));
            break;
        }
        
        while (bytes_read > 0){
            int bytes_written = write(serverSide, buffer, bytes_read);
            bytes_read=bytes_read-bytes_written;
            if (bytes_written <= 0){
                write(1,"Eroare la citirea din fisier\n",sizeof("Eroare la citirea din fisier\n"));
                break;
            }
        }
     }
     write(1,"Fisierul a fost trimis\n",sizeof("Fisierul a fost trimis\n"));
         printf("\n");
    printf("\n");
     user_operation(serverSide,type,serv);
}

void send_report1(int serverSide, int type, struct sockaddr_in serv){
    char buffer[100];
    FILE *f;
    char filename[50];
    write(1,"Nume Fisier\n",sizeof("Nume Fisier\n"));
    scanf("%s",filename);
    f = fopen(filename, "r");
    fscanf(f,"%s",buffer);
    write(serverSide, buffer, 100);
    write(1,"Fisierul a fost trimis\n",sizeof("Fisierul a fost trimis\n"));
    printf("\n");
    printf("\n");
    user_operation(serverSide,type,serv);
}

void send_report2(int serverSide, int type, struct sockaddr_in serv){
    char buffer[1024];
    FILE *f;
    char filename[50];
    write(1,"Nume Fisier\n",sizeof("Nume Fisier\n"));
    scanf("%s",filename);
    f = fopen(filename, "r");
    int n;
    // Sending the data
    while(fgets(buffer, 1024, f) != NULL){
        printf("[SENDING] Data: %s", buffer);
        //n = write(serverSide, buffer, 1024);
        n = sendto(serverSide, buffer, 1024, 0, (struct sockaddr*)&serv, sizeof(serv));
        if (n == -1){
          perror("[ERROR] sending data to the server.");
          exit(1);
        }
        bzero(buffer, 1024);
    }
    strcpy(buffer, "END");
    write(serverSide, buffer, 1024);
    fclose(f);
    
    write(1,"Fisierul a fost trimis\n",sizeof("Fisierul a fost trimis\n"));
    printf("\n");
    printf("\n");
    user_operation(serverSide,type,serv);
}

void fileTransfer(int serverSide, int type, struct sockaddr_in serv){

	char filePath[100];
	char destPath[100];

	char fileBuffer[512];


    write(1,"file1  : ",sizeof("file1  : "));
    scanf("%s",filePath);
    write(1,"destination path : ",sizeof("destination path : "));
    scanf("%s",destPath);

    if(send(serverSide, destPath, strlen(destPath), 0) < 0){
		printf("Could not send destination path to server\n");
	}

    //lock the process of transfering the file
	FILE *file_open = fopen(filePath, "r");
	bzero(fileBuffer, 512);
	int blockSize,  i=0;

	while((blockSize = fread(fileBuffer, sizeof(char), 512, file_open)) > 0){
		printf("data sent %d = %d\n", i, blockSize);
		if(send(serverSide, fileBuffer, blockSize, 0) < 0){
			perror("Error sending file to server");
			exit(1);
		}
		bzero(fileBuffer, 512);
		i++;
	}
	//unlock the write file process
    // close()
}

void user_operation(int serverSide,int type, struct sockaddr_in serv){
    int operation_type,next=0;
    write(1,"Introduceti numarul corespunzator operatiei\n",sizeof("Introduceti numarul corespunzator operatiei\n"));
    write(1,"1 : Depozitare\n",sizeof("1 : Depozitare\n"));
    write(1,"2 : Retragere Numerar\n",sizeof("2 : Retragere Numerar\n"));
    write(1,"3 : Sold Curent\n",sizeof("3 : Sold Curent\n"));
    write(1,"4 : Schimba Parola\n",sizeof("4 : Schimba Parola\n"));
    write(1,"5 : Detaliile Contului\n",sizeof("5 : Detaliile Contului\n"));
    write(1,"6 : Extras de Cont\n",sizeof("6 : Extras de Cont\n"));

    write(1,"9 : File Transfer\n",sizeof("9 : File Transfer\n"));
    write(1,"10 : Iesire\n",sizeof("10 : Iesire\n"));
    scanf("%d",&operation_type);
    switch (operation_type){
        case 1 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            user_deposit(serverSide,type,serv);
            break;
        case 2 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            user_withdraw(serverSide,type,serv);
            break;
        case 3 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            balance_enquiry(serverSide,type,serv);
            break;
        case 4 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            change_user_password(serverSide,type,serv);
            break;
        case 5 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            view_user_details(serverSide,type,serv);
            break;
        case 6 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            checkout(serverSide,type,serv);
            break;
       
        case 9 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            fileTransfer(serverSide,type,serv);
            break;
        case 10 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            Exit(serverSide);
            break;
    
        default: operation_type = 10;
            write(1,"Operatie nedefinita.\n Oprire sesiune...\n",sizeof("Operatie nedefinita.\n Oprire sesiune...\n"));
            write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            Exit(serverSide);
            break;
    }  
}

int main(){
    int serverSide,type,status=0;
    int arr[2];

    // initializare socket
    struct sockaddr_in serv;
    serverSide = socket(AF_INET,SOCK_STREAM,0);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(5555);
    // conectare la server
    connect(serverSide,(struct sockaddr *)&serv,sizeof(serv));
    // executare login()
    login(serverSide,arr);
    type = 2;
    status = arr[1];

    write(serverSide,&type,sizeof(type));

    if((type==1) && (status==1)) // tip Admin
    {
        printf("\n");
        printf("\n");
        write(1,"Utilizator nedefinit\n",sizeof("Utilizator nedefinit\n"));
    }
    if((type==2) && (status==1)) // tip Utilizator
    {  
        printf("\n");
        printf("\n");
        user_operation(serverSide,type,serv);
    }
    return 0;
}

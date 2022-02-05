#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include<stdbool.h>
#include<fcntl.h>


#define SERVER_PATH     "/tmp/server"
#define MAX_MSG		512
#define MIN_MSG		8

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

void admin_operation(int serverSide);

int admin_login(int serverSide){
    admin new_admin;
    int status=0;

    // Input de la utilizator
    write(1,"Id : ",sizeof("Id : "));
    scanf("%d",&new_admin.adminid);
    write(1,"Parola : ",sizeof("Parola : "));
    scanf("%s",new_admin.password);

    // Trimite inputul catre server
    write(serverSide,&new_admin.adminid,sizeof(new_admin.adminid));
    write(serverSide,&new_admin.password,sizeof(new_admin.password));

    // Primeste statusul de la server
    read(serverSide,&status,sizeof(int));
    return status;
}

void login(int serverSide,int arr[]){
    int type = 1,status;


    // write(1,"Autentificare\nAlegeti Tipul de Utilizator\n",sizeof("Autentificare\nAlegeti Tipul de Utilizator\n"));
    // write(1,"1 : Admin\n",sizeof("1 : Admin\n"));
    // write(1,"2 : Utilizator Normal\n",sizeof("2 : Utilizator Normal\n"));

    // Input tipul de utilizator
    // scanf("%d",&type);

    switch (type){
    case 1 : write(serverSide,&type,sizeof(type)); // Trimite tipul utilizatorului catre server
        status = admin_login(serverSide);
        break;
    case 2 : write(serverSide,&type,sizeof(type)); // Trimite tipul utilizatorului catre server
        write(1,"Utilizator nedefinit\n",sizeof("Utilizator nedefinit\n"));
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



void add_new_user(int serverSide){
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
    admin_operation(serverSide);
}

void add_admin(int serverSide){
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

    admin_operation(serverSide);
}

void delete_normal_user(int serverSide){
    int status=0,id;

    // Input Utilizator {ID}
    write(1,"Introduceti id utilizatorului pe care doriti sa il stergeti\n",sizeof("Introduceti id utilizatorului pe care doriti sa il stergeti\n"));
    scanf("%d",&id);

    // Trimite Id catre server
    write(serverSide,&id,sizeof(int));

    // Citeste status
    read(serverSide,&status,sizeof(int));

    if(status) write(1,"Utilizator Sters\n",sizeof("Utilizator Sters\n"));
    else write(1,"Eroare \n",sizeof("Eroare \n"));
    printf("\n");
    printf("\n");

    admin_operation(serverSide);
}

void search_normal_user(int serverSide){
    int id,status=0;
    user new_user;
    // Input Utilizator {ID}
    write(1,"Introduceti id-ul utilizatorului\n",sizeof("Introduceti id-ul utilizatorului\n"));
    scanf("%d",&id);
     // Trimite Id catre server
    write(serverSide,&id,sizeof(id));
    read(serverSide,&status,sizeof(int));
    // Citeste status
    read(serverSide,&new_user,sizeof(user));
    if(status){
        printf("ID Utilizator = %d\n",new_user.userid);
		printf("Nume de utilizator = %s\n",new_user.username);
		printf("Numarul Contului =%d\n",new_user.account_num);
		printf("Sold = %f\n",new_user.balance);
    }
    else write(1,"Utilizatorul nu a fost gasit\n",sizeof("Utilizatorul nu a fost gasit\n"));
    printf("\n");
    printf("\n");

    admin_operation(serverSide);
}

void modify_user_details(int serverSide){
    user new_user,other_user;
    int status=0;
    // Input Utilizator {ID}
	write(1,"Introduceti id-ul utilizatorului\n",sizeof("Introduceti id-ul utilizatorului\n"));
	scanf("%d",&new_user.userid);
	write(1,"Introduceti numele de utilizator nou\n",sizeof("Introduceti numele de utilizator nou\n"));
	scanf(" %[^\n]",new_user.username);
	write(1,"Introduceti parola noua\n",sizeof("Introduceti parola noua\n"));
	scanf(" %[^\n]",new_user.password);
    // Trimite Id catre server
    write(serverSide,&new_user,sizeof(new_user));
    // Citeste status
    read(serverSide,&status,sizeof(status));
    if(status) write(1,"Modificarea a fost efectuata\n",sizeof("Modificarea a fost efectuata\n"));
    else write(1,"Eroare ~ verificati id si parola\n",sizeof("Eroare ~ verificati id si parola\n"));
    printf("\n");
    printf("\n");    
    
    admin_operation(serverSide);
}

void change_admin_password(int serverSide){
    char old_pass[20],new_pass[20];
    int status=0;
    // Input Utilizator {ID}
    write(1,"Introduceti parola veche\n",sizeof("Introduceti parola veche\n"));
    scanf(" %[^\n]",old_pass);
    write(1,"Introduceti parola noua\n",sizeof("Introduceti parola noua\n"));
    scanf(" %[^\n]",new_pass);
    // Trimite Id catre server
    write(serverSide,&old_pass,sizeof(old_pass));
    write(serverSide,&new_pass,sizeof(new_pass));
    // Citeste status
    read(serverSide,&status,sizeof(int));
    if(status) write(1,"Modificarea a fost efectuata\n",sizeof("Modificarea a fost efectuata\n"));
    else write(1,"Eroare ~ verificati id si parola\n",sizeof("Eroare ~ verificati id si parola\n"));
        printf("\n");
    printf("\n");
    admin_operation(serverSide);
}

void Exit(int serverSide){
    write(1,"Deconectare...\n",sizeof("Deconectare...\n"));
    close(serverSide);
    exit(0);
}

void admin_operation(int serverSide){
    int operation_type,next=0;
    write(1,"Introduceti numarul corespunzator operatiei\n",sizeof("Introduceti numarul corespunzator operatiei\n"));
    write(1,"1 : Creaza cont utilizator\n",sizeof("1 : Creaza cont utilizator\n"));
    write(1,"2 : Sterge utilizator\n",sizeof("3 : Sterge utilizator\n"));
    write(1,"3 : Cauta utilizator\n",sizeof("5 : Cauta utilizator\n"));
    write(1,"4 : Modifica datele utilizator\n",sizeof("7 : Modifica datele utilizator\n"));
    write(1,"5 : Schimba parola\n",sizeof("9 : Schimba parola\n")); 
    write(1,"7 : Add Admin\n",sizeof("7 : Add Admin\n"));   
    write(1,"10 : Iesire\n",sizeof("6 : Iesire\n"));
    scanf("%d",&operation_type);

    switch (operation_type){
        case 1 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            add_new_user(serverSide);
            break;
        case 2 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            delete_normal_user(serverSide);
            break;
        case 3 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            search_normal_user(serverSide);
            break;
        case 4 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            modify_user_details(serverSide);
            break;
        case 5 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            change_admin_password(serverSide);
            break;
         case 7 : write(serverSide,&operation_type,sizeof(operation_type)); // Trimite operatia catre server
            add_admin(serverSide);
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

int main(int argc, char *argv[]) {
    int arr[2];
    int    serverSide, err, numBytes, rc;
    struct sockaddr_un server_addr, client_addr;
    int type, status;

    serverSide = socket(AF_UNIX, SOCK_STREAM, 0);
    if(serverSide < 0){
        perror("socket fail");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_PATH);

    err = connect(serverSide, (struct sockaddr *)&server_addr, SUN_LEN(&server_addr));
        if (err < 0) {
        perror("\n Failed to connect to UNIX server");
    } else {
        printf("\n Clientul de administrare server UNIX conectat\n");
    }


    login(serverSide,arr);
    type = 1;
    status = arr[1];

    write(serverSide,&type,sizeof(type));

    if((type==1) && (status==1)) // tip Admin
    {
        printf("\n");
        printf("\n");
        admin_operation(serverSide);
    }
    if((type==2) && (status==1)) // tip Utilizator
    {  
        printf("\n");
        printf("\n");
          write(1,"Utilizator nedefinit\n",sizeof("Utilizator nedefinit\n"));
    }
    return 0;

    

   
}

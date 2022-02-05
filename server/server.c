
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stddef.h>
#include <errno.h>
#include <sys/un.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>



///Serverul si clientul de administrare comunica prin acest fisier
#define SERVER_PATH "/tmp/server"

#define NUM_MAX_CLIENTI 512

#define MAXCHAR 512

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


// 	UNIX server thread



void check_admin(int clientSide,int id,char password[30]){
    int status=0,fd;
    admin new_admin;
    fd = open("AdminDB.txt",O_RDONLY);
    while(read(fd,&new_admin,sizeof(new_admin))){
        if(!strcmp(new_admin.password,password) && (new_admin.adminid==id)){
            status = 1;
            write(1,"Admin logat\n",sizeof("Admin logat\n"));
            //printf("Utilizator logat [Admin Id: %d]", id);
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(int));
}

void check_user(int clientSide,int id,char password[30]){
    int status=0,fd;
    user new_user;
    fd = open("UserDB.txt",O_RDONLY);
    while(read(fd,&new_user,sizeof(user))){
        if(!strcmp(new_user.password,password) && (new_user.userid==id) && (new_user.is_deleted==false)){
            status = 1;
            write(1,"Utilizator logat\n",sizeof("Utilizator logat\n"));
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(int));
}

void add_normal_user(int clientSide){
    user previous__user,new_user;
    int status = 0,id=0,acc=10000;
    read(clientSide,&new_user,sizeof(user));
    int fd = open("UserDB.txt",O_RDWR);
    lseek(fd,-(sizeof(user)),SEEK_END);
    read(fd,&previous__user,sizeof(user));
    id = previous__user.userid;
    acc = previous__user.account_num;
    new_user.userid = id+1;
    new_user.account_num = acc+1;
    srand(time(NULL));
    float scale = rand()/(float) RAND_MAX;
    new_user.balance = scale*(50000);
    new_user.is_deleted = false;
    write(1,"[Admin] ~ Creaza Utilizator\n",sizeof("[Admin] ~ Creaza Utilizator\n"));
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = -sizeof(user);
    lock.l_len = sizeof(user);
    lock.l_pid = getpid();
    int lock_status = fcntl(fd,F_SETLKW,&lock);
    write(1,"[Admin] ~ Operatia se executa\n",sizeof("[Admin] ~ Operatia se executa\n"));
    // sleep(10);
    write(fd,&new_user,sizeof(user));
    lock.l_type = F_UNLCK;
    lock_status = fcntl(fd,F_SETLKW,&lock);
    write(1,"[Admin] ~ Operatie terminata\n",sizeof("[Admin] ~ Operatie terminata\n"));
    if(lock_status!=-1) status =1;
    close(fd);
    write(clientSide,&new_user,sizeof(user));
    write(clientSide,&status,sizeof(int));
}

void add_admin(int clientSide){
    admin previous__user,new_user;
    int status = 0,id=0;
    read(clientSide,&new_user,sizeof(user));
    int fd = open("AdminDB.txt",O_RDWR);
    lseek(fd,-(sizeof(user)),SEEK_END);
    read(fd,&previous__user,sizeof(user));
    id = previous__user.adminid;
    new_user.adminid = id+1;
    srand(time(NULL));
    float scale = rand()/(float) RAND_MAX;
    write(1,"[Admin] ~ Creaza Admin\n",sizeof("[Admin] ~ Creaza Admin\n"));
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = -sizeof(user);
    lock.l_len = sizeof(user);
    lock.l_pid = getpid();
    int lock_status = fcntl(fd,F_SETLKW,&lock);
    write(1,"[Admin] ~ Operatia se executa\n",sizeof("[Admin] ~ Operatia se executa\n"));
    // sleep(10);
    write(fd,&new_user,sizeof(user));
    lock.l_type = F_UNLCK;
    lock_status = fcntl(fd,F_SETLKW,&lock);
    write(1,"[Admin] ~ Operatie terminata\n",sizeof("[Admin] ~ Operatie terminata\n"));
    if(lock_status!=-1) status =1;
    close(fd);
    write(clientSide,&new_user,sizeof(user));
    write(clientSide,&status,sizeof(int));
}

void delete_normal_user(int clientSide){
    int id,status=0;
    user new_user;
    read(clientSide,&id,sizeof(int));
    int fd = open("UserDB.txt",O_RDWR);
    while(read(fd,&new_user,sizeof(new_user))){
        if(new_user.is_deleted == false && new_user.userid==id){
            new_user.is_deleted = true;
            write(1,"[Admin] ~ Sterge Utilizator\n",sizeof("[Admin] ~ Sterge Utilizator\n"));
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(user));
            lock.l_len = sizeof(user);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Admin] ~ Operatia se executa\n",sizeof("[Admin] ~ Operatia se executa\n"));
            lseek(fd,-(sizeof(user)),SEEK_CUR);
            write(fd,&new_user,sizeof(user));
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Admin] ~ Operatie terminata\n",sizeof("[Admin] ~ Operatie terminata\n"));
            status = 1;
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(int));
}

void search_normal_user(int clientSide){
    int id,status=0;
    user new_user;
    read(clientSide,&id,sizeof(int));
    int fd = open("UserDB.txt",O_RDONLY);
    while(read(fd,&new_user,sizeof(new_user))){
        if((new_user.userid==id) && (new_user.is_deleted==false)){
            write(1,"[Admin] ~ Cauta Utilizator\n",sizeof("[Admin] ~ Cauta Utilizator\n"));
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(user));
            lock.l_len = sizeof(user);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Admin] ~ Operatia se executa\n",sizeof("[Admin] ~ Operatia se executa\n"));
            lseek(fd,-(sizeof(user)),SEEK_CUR);
            read(fd,&new_user,sizeof(user));
            // sleep(10);
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Admin] ~ Operatie terminata\n",sizeof("[Admin] ~ Operatie terminata\n"));
            status = 1;
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(status));
    write(clientSide,&new_user,sizeof(new_user));
}

void modify_user_details(int clientSide){
    int id,status=0;
    user new_user,old_user;
    read(clientSide,&new_user,sizeof(user));
    int fd = open("UserDB.txt",O_RDWR);
    // lseek(fd,(new_user.userid-1)*(sizeof(user)),SEEK_SET);
    while(read(fd,&old_user,sizeof(old_user))){
        if((old_user.userid==new_user.userid) && (old_user.is_deleted==false)){
            write(1,"[Admin] ~ Modifica Utilizator\n",sizeof("[Admin] ~ Modifica Utilizator\n"));
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(user));
            lock.l_len = sizeof(user);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Admin] ~ Operatia se executa\n",sizeof("[Admin] ~ Operatia se executa\n"));
            lseek(fd,-(sizeof(user)),SEEK_CUR);
            strcpy(old_user.username,new_user.username);
            strcpy(old_user.password,new_user.password);
            write(fd,&old_user,sizeof(user));
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Admin] ~ Operatie terminata\n",sizeof("[Admin] ~ Operatie terminata\n"));
            status = 1;
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(status));
}

void change_password(int clientSide,int id){
    admin new_admin;
    int status=0;
    char password[20],new_pass[20];
    // read(clientSide,&id,sizeof(int));
    read(clientSide,&password,sizeof(password));
    read(clientSide,&new_pass,sizeof(new_pass));
    int fd = open("AdminDB.txt",O_RDWR);
    while(read(fd,&new_admin,sizeof(new_admin))){
        if((new_admin.adminid==id) && !strcmp(new_admin.password,password)){
            write(1,"[Admin] ~ Schimba Parola\n",sizeof("[Admin] ~ Schimba Parola\n"));
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(admin));
            lock.l_len = sizeof(admin);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Admin] ~ Operatia se executa\n",sizeof("[Admin] ~ Operatia se executa\n"));
            lseek(fd,-(sizeof(admin)),SEEK_CUR);
            strcpy(new_admin.password,new_pass);
            write(fd,&new_admin,sizeof(admin));
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Admin] ~ Operatie terminata\n",sizeof("[Admin] ~ Operatie terminata\n"));
            status = 1;
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(status));
}

void user_deposit(int clientSide,int id){
    user new_user;
    float amount;
    int status=0;
    read(clientSide,&amount,sizeof(float));
    int fd = open("UserDB.txt",O_RDWR);
    while(read(fd,&new_user,sizeof(new_user))){
        if(new_user.is_deleted == false && new_user.userid==id){
            write(1,"[Utilizator] ~ Depozit\n",sizeof("[Utilizator] ~ Depozit\n"));
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(user));
            lock.l_len = sizeof(user);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Utilizator] ~ Operatia se executa\n",sizeof("[Utilizator] ~ Operatia se executa\n"));
            lseek(fd,(sizeof(user)),SEEK_CUR);
            new_user.balance = new_user.balance + amount;
            write(fd,&new_user,sizeof(user));
            getchar();
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Utilizator] ~ Operatie terminata\n",sizeof("[Utilizator] ~ Operatie terminata\n"));
            status = 1;
            break;
        }
    }
    close(fd);
    write(clientSide,&new_user.balance,sizeof(new_user.balance));
    write(clientSide,&status,sizeof(int));
}

void user_withdraw(int clientSide,int id){
    user new_user;
    float amount;
    int status=0;
    read(clientSide,&amount,sizeof(float));
    int fd = open("UserDB.txt",O_RDWR);
    while(read(fd,&new_user,sizeof(new_user))){
        if(new_user.is_deleted == false && new_user.userid==id){
            write(1,"[Utilizator] ~ Retrage Numerar\n",sizeof("[Utilizator] ~ Retrage Numerar\n"));
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(user));
            lock.l_len = sizeof(user);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Utilizator] ~ Operatia se executa\n",sizeof("[Utilizator] ~ Operatia se executa\n"));
            lseek(fd,(sizeof(user)),SEEK_CUR);
            new_user.balance = new_user.balance - amount;
            if(new_user.balance<0) status = 0;
            else
            {
                write(fd,&new_user,sizeof(user));
                status = 1;
            }
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Utilizator] ~ Operatie terminata\n",sizeof("[Utilizator] ~ Operatie terminata\n"));
            break;
        }
    }
    close(fd);
    write(clientSide,&new_user.balance,sizeof(new_user.balance));
    write(clientSide,&status,sizeof(int));
}

void user_balance(int clientSide,int id){
    user new_user;
    int status =0;
    int fd = open("UserDB.txt",O_RDONLY);
    while(read(fd,&new_user,sizeof(new_user))){
        if((new_user.userid==id) && (new_user.is_deleted==false))
        {
            write(1,"[Utilizator] ~ Sold Cont\n",sizeof("[Utilizator] ~ Sold Cont\n"));
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(user));
            lock.l_len = sizeof(user);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Utilizator] ~ Operatia se executa\n",sizeof("[Utilizator] ~ Operatia se executa\n"));
            lseek(fd,-(sizeof(user)),SEEK_CUR);
            read(fd,&new_user,sizeof(user));
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Utilizator] ~ Operatie terminata\n",sizeof("[Utilizator] ~ Operatie terminata\n"));
            status = 1;
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(status));
    write(clientSide,&new_user.balance,sizeof(new_user.balance));

}

void user_password(int clientSide,int id){
    user new_user;
    int status=0;
    char password[20],new_pass[20];
    read(clientSide,&password,sizeof(password));
    read(clientSide,&new_pass,sizeof(new_pass));
    int fd = open("UserDB.txt",O_RDWR);
    while(read(fd,&new_user,sizeof(user))){
        if((new_user.userid==id) && !strcmp(new_user.password,password)){
            write(1,"[Utilizator] ~ Schimba Parola\n",sizeof("[Utilizator] ~ Schimba Parola\n"));
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(user));
            lock.l_len = sizeof(user);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Utilizator] ~ Operatia se executa\n",sizeof("[Utilizator] ~ Operatia se executa\n"));
            lseek(fd,-(sizeof(user)),SEEK_CUR);
            strcpy(new_user.password,new_pass);
            write(fd,&new_user,sizeof(user));
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Utilizator] ~ Operatie terminata\n",sizeof("[Utilizator] ~ Operatie terminata\n"));
            status = 1;
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(status));
}

void view_user_details(int clientSide,int id){
    int status=0;
    user new_user;
    int fd = open("UserDB.txt",O_RDONLY);
    while(read(fd,&new_user,sizeof(new_user))){
        if((new_user.userid==id) && (new_user.is_deleted==false)){
            write(1,"[Utilizator] ~ Detalii Cont\n",sizeof("[Utilizator] ~ Detalii Cont\n"));
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(user));
            lock.l_len = sizeof(user);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Utilizator] ~ Operatia se executa\n",sizeof("[Utilizator] ~ Operatia se executa\n"));
            lseek(fd,-(sizeof(user)),SEEK_CUR);
            read(fd,&new_user,sizeof(user));
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Utilizator] ~ Operatie terminata\n",sizeof("[Utilizator] ~ Operatie terminata\n"));
            status = 1;
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(status));
    if(status) write(clientSide,&new_user,sizeof(new_user));
}

void checkout(int clientSide,int id){
    int status=0;
    user new_user;
    int fd = open("UserDB.txt",O_RDONLY);
    while(read(fd,&new_user,sizeof(new_user))){
        if((new_user.userid==id) && (new_user.is_deleted==false)){
            write(1,"[Utilizator] ~ Extras Cont\n",sizeof("[Utilizator] ~ Extras Cont\n"));
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -(sizeof(user));
            lock.l_len = sizeof(user);
            lock.l_pid = getpid();
            int lock_status = fcntl(fd,F_SETLKW,&lock);
            write(1,"[Utilizator] ~ Operatia se executa\n",sizeof("[Utilizator] ~ Operatia se executa\n"));
            lseek(fd,-(sizeof(user)),SEEK_CUR);
            read(fd,&new_user,sizeof(user));
            lock.l_type = F_UNLCK;
            lock_status = fcntl(fd,F_SETLK,&lock);
            write(1,"[Utilizator] ~ Operatie terminata\n",sizeof("[Utilizator] ~ Operatie terminata\n"));
            status = 1;
            break;
        }
    }
    close(fd);
    write(clientSide,&status,sizeof(status));
    if(status) write(clientSide,&new_user,sizeof(new_user));
    time_t now = time(NULL);
    char filename[50]="log.txt";    
    FILE *output_file;
    output_file = fopen(filename, "w");
    fprintf(output_file,"Timp: %s", ctime(&now));
    fprintf(output_file,"Nume de utilizator: %s \t", new_user.username);
    fprintf(output_file,"Cont: %d \t", new_user.account_num);
    fprintf(output_file,"Extras de Cont\n");
    fclose(output_file);    
}

void get_report(int clientSide){
    write(1,"[Utilizator] ~ Raport\n",sizeof("[Utilizator] ~ Raport\n"));
    FILE *output_file;
    char client_message[2000];
    int read_size = 0;
    output_file = fopen("raport.txt", "w");
    time_t now = time(NULL);
    printf("inainte de while");
    fprintf(output_file,"Timp: %s", ctime(&now));
    while((read_size = recv(clientSide, &client_message, sizeof(client_message),0)) > 0) {
    	 fprintf(output_file,"%s", client_message);
    	 printf("Read Text: %.*s", read_size, client_message);
    }
    if(read_size == 0){
        printf("Fisier citit\n");
    }
    fclose(output_file);
}

void get_report1(int clientSide){
    char buffer[100];
    FILE *f;
    read(clientSide, buffer, 100);
    f = fopen("raport.txt", "w");
    fprintf(f, "%s", buffer);
    printf("Fisier citit\n");
}

void get_report2(int clientSide,int sockfd, struct sockaddr_in addr ){
    char buffer[1024];
    FILE *f;
    f = fopen("raport.txt", "w");
    int n;
    socklen_t addr_size;
    while(true){    
        addr_size = sizeof(addr);
        //n = read(clientSide, buffer, 1024);
        n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&addr, &addr_size);
        if(strcmp(buffer, "END") == 0){
            break;
        }
        printf("Data: %s", buffer);
        fprintf(f,"%s", buffer);
    }
    fclose(f);
    printf("Fisier citit\n");
}

void receiveFromUser(int clientSide) {
    char file_buffer[512];
    char *file_path = "";
    write(1, "start\n", sizeof("start\n"));
    memset(buffer, 0, BUFFSIZE);
    if(recv(clientSide, buffer, BUFFSIZE, 0) < 0){
        //destination read error
        printf("destination of file cannot be read\n");
    }
    write(1, "file destination\n", sizeof("file destination\n"));
    printf("file destination: %s\n", buffer);
    file_path = buffer;
    write(1, "openFile\n", sizeof("openFile\n"));
    FILE *file_open;
    fopen(file_path, "w+");
    if(file_open == NULL){
        printf("File %s cannot be opened on server\n", file_path);
    }
    else{
        
        bzero(file_buffer, 512);
        int block_size = 0, i=0;

        while((block_size = recv(clientSide, file_buffer, 512, 0)) > 0){
            printf("Data Received %d = %d\n", i, block_size);
            int write_sz = fwrite(file_buffer, sizeof(char), block_size, file_open);
            bzero(file_buffer, 512);
            i++;
        }
    }
    write(1, "transfer complet\n", sizeof("transfer complet\n"));
    printf("Transfer completed successfully\n");
    fclose(file_open);
    close(clientSide);
}


void Exit(int clientSide){
    write(1,"Un client s-a deconectat\n",sizeof("Un client s-a deconectat\n"));
    close(clientSide);
    exit(0);
}

void * unix_main (void *args) {

    int user_type,id,check,login_num=0,admin_option,user_option,num;
    char password[20];
    printf("UNIX server Start\n");

    int serverSide, clientSide, ch, err;
    struct sockaddr_un server_connection;

    ///AF_UNIX specifica tipul socket-ului care poate fi pt internet, UNIX etc
    serverSide = socket(AF_UNIX, SOCK_STREAM, 0); //comunicare pe device cu protocolul TCP(SOCK_STREAM)
    if (serverSide < 0) {
        perror("UNIX socket() error");
    }

    if(serverSide < 0){
    write(1,"fail to socket\n",sizeof("fail to socket\n"));        
    }

    memset(&server_connection, 0, sizeof(server_connection));
    server_connection.sun_family = AF_UNIX;
    strcpy(server_connection.sun_path, SERVER_PATH);
    
    unlink(SERVER_PATH);
    ///bind stabileste legatura dintre serverSide si structura serverAddr
    err = bind(serverSide, (struct sockaddr *)&server_connection, SUN_LEN(&server_connection));
    if (err < 0) {
        perror("UNIX bind() error");
    }

    err = listen(serverSide, 10);    ///serverul este trecut in mod ascultare
    if (err < 0) {
        perror("UNIX listen() error");
    }
    // clientSide = accept(serverSide, NULL, NULL);

    while(true){   
        clientSide = accept(serverSide, NULL, NULL);
        if(!fork()){
            write(1,"Un client incearca sa se conecteze\n",sizeof("Un client incearca sa se conecteze\n"));
            read(clientSide,&user_type,sizeof(int));
            while(true){
                if(user_type==1){
                    admin new_admin;
                    read(clientSide,&id,sizeof(new_admin.adminid));
                    read(clientSide,&password,sizeof(new_admin.password));
                    check_admin(clientSide,id,password);
                    break;
                }
                else if(user_type==2){
                    user new_user;
                    read(clientSide,&id,sizeof(new_user.userid));
                    read(clientSide,&password,sizeof(new_user.password));
                    check_user(clientSide,id,password);
                    break;
                }
                else{
                    write(1,"Eroare de conectare\n",sizeof("Eroare de conectare\n"));
                    break;
                }
            }
            while(true){
                if(user_type==1){
                    write(1,"Admin Menu\n",sizeof("Admin Menu\n"));
                    read(clientSide,&admin_option,sizeof(int));
                    if(admin_option==1) add_normal_user(clientSide);
                    else if(admin_option==2) delete_normal_user(clientSide);
                    else if(admin_option==3) search_normal_user(clientSide);
                    else if(admin_option==4) modify_user_details(clientSide);
                    else if(admin_option==5) change_password(clientSide,id);
                    else if(user_option==7) {add_admin(clientSide); write(1,"Initialize Add Admin\n",sizeof("Initialize Add Admin\n"));}
                    else if(admin_option==10) Exit(clientSide);
                }
                else if(user_type==2){
                    write(1,"User Menu\n",sizeof("User Menu\n"));
                    read(clientSide,&user_option,sizeof(int));
                    if(user_option==1) user_deposit(clientSide,id);
                    else if(user_option==2) user_withdraw(clientSide,id);
                    else if(user_option==3) user_balance(clientSide,id);
                    else if(user_option==4) user_password(clientSide,id);
                    else if(user_option==5) view_user_details(clientSide,id);
                    else if(user_option==6) checkout(clientSide,id);
                    else if(user_option==7) {add_admin(clientSide); write(1,"Initialize Add Admin\n",sizeof("Initialize Add Admin\n"));}
                    else if(user_option==8) add_normal_user(clientSide);
                    else if(user_option==10) Exit(clientSide);
                }
            }
        }else close(clientSide);
    }

    if (serverSide > 0)
        close(serverSide);
    if (clientSide > 0)
        close(clientSide);

    unlink(SERVER_PATH); ///fisierul de comunicare este inchis
}


//	------------------------------------------------------------------
// INET server thread
///punctul de intrare pt thread ul Inet

void * inet_main(void * arg) {

    int user_type,id,check,login_num=0,admin_option,user_option,num;
    char password[20];
    
    printf("INET Server Start\n");

    int serverSide, clientSide;
    struct sockaddr_in server_connection, client_connection;

    // setup clients server configuration
    serverSide = socket(AF_INET,SOCK_STREAM,0);
    server_connection.sin_family = AF_INET;
    server_connection.sin_addr.s_addr = INADDR_ANY;
    server_connection.sin_port = htons(5555);

    //	this testing block is used to detect the socket error
    if (serverSide < 0) {
        printf("INET socket() failed\n");
    }

    bind(serverSide,(struct sockaddr*)&server_connection,sizeof(server_connection));
    listen(serverSide,2);

    while(true){   
        int len = sizeof(client_connection);
        clientSide = accept(serverSide,(struct sockaddr *)&client_connection,&len);
        if(!fork()){
            write(1,"Un client incearca sa se conecteze\n",sizeof("Un client incearca sa se conecteze\n"));
            read(clientSide,&user_type,sizeof(int));
            while(true){
                if(user_type==1){
                    admin new_admin;
                    read(clientSide,&id,sizeof(new_admin.adminid));
                    read(clientSide,&password,sizeof(new_admin.password));
                    check_admin(clientSide,id,password);
                    break;
                }
                else if(user_type==2){
                    user new_user;
                    read(clientSide,&id,sizeof(new_user.userid));
                    read(clientSide,&password,sizeof(new_user.password));
                    check_user(clientSide,id,password);
                    break;
                }
                else{
                    write(1,"Eroare de conectare\n",sizeof("Eroare de conectare\n"));
                    break;
                }
            }
            while(true){
                if(user_type==1){
                    write(1,"Admin Menu\n",sizeof("Admin Menu\n"));
                    read(clientSide,&admin_option,sizeof(int));
                    if(admin_option==1) add_normal_user(clientSide);
                    else if(admin_option==2) delete_normal_user(clientSide);
                    else if(admin_option==3) search_normal_user(clientSide);
                    else if(admin_option==4) modify_user_details(clientSide);
                    else if(admin_option==5) change_password(clientSide,id);
                    else if(admin_option==10) Exit(clientSide);
                }
                else if(user_type==2){
                    write(1,"User Menu\n",sizeof("User Menu\n"));
                    read(clientSide,&user_option,sizeof(int));
                    if(user_option==1) user_deposit(clientSide,id);
                    else if(user_option==2) user_withdraw(clientSide,id);
                    else if(user_option==3) user_balance(clientSide,id);
                    else if(user_option==4) user_password(clientSide,id);
                    else if(user_option==5) view_user_details(clientSide,id);
                    else if(user_option==6) checkout(clientSide,id);
                    else if(user_option==9) receiveFromUser(clientSide);
                    else if(user_option==10) Exit(clientSide);
                }
            }
        }else close(clientSide);
    }
}

int main () {

    int inetPort;
    pthread_t
    unixThread, 	// UNIX Thread: componenta UNIX
    inetThread; 	// INET Thread: componenta INET
    
    unlink(SERVER_PATH);
    pthread_create (&unixThread, NULL, unix_main, NULL) ;

    inetPort = 5555;
    pthread_create (&inetThread, NULL, inet_main, &inetPort) ;

    pthread_join (unixThread, NULL);
    pthread_join (inetThread, NULL);

    unlink(SERVER_PATH) ;
    return 0 ;
}

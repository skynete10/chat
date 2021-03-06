#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include </root/cryptage.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sqlite3.h>
#define SERVERIP "127.0.0.1"
#define SERVERPORT 8080

#define BUFFSIZE 1024
#define userLEN 32
#define OPTLEN 16
#define FILEBUFF 20000
#define LINEBUFF 2048
#define NAMESIZE 16
#define LENGTH 512
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define pink "\e[35m"
struct PACKET {
char option[OPTLEN]; 
char user[userLEN]; 
char buff[BUFFSIZE]; // message size
char name[NAMESIZE];
};

struct USER {
int sockfd; // user's socket file descriptor: objets génériques avec des méthodes génériques (open(), close(), read(), write(), ...).
char user[userLEN]; // user's name
char tel[32];
int age[16];
char address[32];
};
struct THREADINFO {
pthread_t thread_ID; // thread's pointer:récupérer l'ID d'un thread
int sockfd; // socket file descriptor
};
int isconnected, sockfd;
char option[LINEBUFF];
struct USER me;
int num;
int connect_with_server();
void setuser(struct USER *me);//set username
void logout(struct USER *me);//desconnecter de serveur
void profile(struct USER *me, char *msg);
void login(struct USER *me);//login
void *receiver(void *param);//receive message
void sendtoall(struct USER *me, char *msg); //broadcast
void sendtoclient(struct USER *me, char * target, char *msg);//send to specific user
void update_name(struct USER *me,char *target, char *msg);
void sendencrypt(struct USER *me, char * target, char *msg);
void sendfile(struct USER *me, char * target, char *fname,char *buff);
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
      printf("------------------------------------\n");
   for(i=0; i<argc; i++){
   	
      printf(ANSI_COLOR_CYAN "|%s               %s             \n" ANSI_COLOR_RESET, azColName[i], argv[i] ? argv[i] : "NULL");
      printf("------------------------------------\n");
   }
   printf("\n");
   return 0;
}

int main(int argc, char **argv) {
	  sqlite3 *db;
   char *zErrMsg = 0;
   int  rc;
   char *sql;

   rc = sqlite3_open("chat.db", &db);

   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Base de données [chat.db] ouvert avec succès\n");
   }
int sockfd, userlen;
memset(&me, 0, sizeof(struct USER));
while(gets(option)) {
if(!strncmp(option, "exit", 4)) {
logout(&me);
break;
}
else if(!strncmp(option, "login", 5)) {
char *ptr = strtok(option, " ");
ptr = strtok(0, " ");//set ptr as username
memset(me.user, 0, sizeof(char) * userLEN);
if(ptr != NULL) {
userlen = strlen(ptr);//strlen:calcule la longeur du chaine de caractere
if(userlen > userLEN) ptr[userLEN] = 0;
strcpy(me.user, ptr);//copy ptr in me.user
}
else {

strcpy(me.user, "default");//copy default to me.user



}
login(&me);
}
else if(!strncmp(option, "change", 6)) {
char *ptr = strtok(option, " ");
ptr = strtok(0, " ");
memset(me.user, 0, sizeof(char) * userLEN);
if(ptr != NULL) {
userlen = strlen(ptr);
if(userlen > userLEN) ptr[userLEN] = 0;
strcpy(me.user, ptr);
setuser(&me);
}
}
else if(!strncmp(option, "specf", 5)) {
char *ptr = strtok(option, " ");
char temp[userLEN];
ptr = strtok(0, " ");
memset(temp, 0, sizeof(char) * userLEN);
if(ptr != NULL) {
userlen = strlen(ptr);
if(userlen > userLEN) ptr[userLEN] = 0;
strcpy(temp, ptr);
while(*ptr) ptr++; ptr++;
while(*ptr <= ' ') ptr++;
sendtoclient(&me, temp, ptr);
}
}
else if(!strncmp(option, "sfile", 5)) {
char *ptr = strtok(option, " ");
char temp[userLEN];
char *buff;
ptr = strtok(0, " ");
memset(temp, 0, sizeof(char) * userLEN);
if(ptr != NULL) {
userlen = strlen(ptr);
if(userlen > userLEN) ptr[userLEN] = 0;
strcpy(temp, ptr);
while(*ptr) ptr++; ptr++;
while(*ptr <= ' ') ptr++;
sendfile(&me, temp, ptr,buff);
}
}
else if(!strncmp(option, "encrypt", 7)) {
char *ptr = strtok(option, " ");
char temp[userLEN];
ptr = strtok(0, " ");
memset(temp, 0, sizeof(char) * userLEN);
if(ptr != NULL) {
userlen = strlen(ptr);
if(userlen > userLEN) ptr[userLEN] = 0;
strcpy(temp, ptr);
while(*ptr) ptr++; ptr++;
while(*ptr <= ' ') ptr++;
sendencrypt(&me, temp, ptr);
}
}
else if(!strncmp(option, "broadcast", 9)) {
sendtoall(&me, &option[9]);
}
else if(!strncmp(option, "update", 6)) {
char *ptr = strtok(option, " ");
char temp[userLEN];
ptr = strtok(0, " ");
memset(temp, 0, sizeof(char) * userLEN);
if(ptr != NULL) {
userlen = strlen(ptr);
if(userlen > userLEN) ptr[userLEN] = 0;
strcpy(temp, ptr);
while(*ptr) ptr++; ptr++;
while(*ptr <= ' ') ptr++;

update_name(&me, temp, ptr);
}
}
else if(!strncmp(option, "list", 4)) {
 sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data = "Callback function called";

   /* Open database */
   rc = sqlite3_open("chat.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }

   /* Create SQL statement */
   sql = "SELECT * from users";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Operation done successfully\n");
   }
  
  
}
else if(!strncmp(option, "logout", 6)) {
logout(&me);
}
else if(!strncmp(option, "profil", 6)) {
profile(&me, &option[6]);
}
else fprintf(stderr, "Unknown option...\n");
}
return 0;
}

void login(struct USER *me) {

	
int recvd;
if(isconnected) {
fprintf(stderr, "You are already connected to server at %s:%d\n", SERVERIP, SERVERPORT);
return;
}
sockfd = connect_with_server();
if(sockfd >= 0) {
isconnected = 1;
me->sockfd = sockfd;
if(strcmp(me->user, "default")) setuser(me);

char term;
fprintf(stderr, ANSI_COLOR_GREEN "please enter your phone number:\n" ANSI_COLOR_RESET);
if(scanf("%d%c", &num, &term) != 2 || term != '\n')
    printf("failure\n");
else
    printf("valid integer followed by enter key\n");
printf(pink "%s est Connecté\n" ANSI_COLOR_RESET, me->user);

 sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char sql[900]=" ";

   /* Open database */
   rc = sqlite3_open("chat.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }

   /* Create SQL statement */
   sprintf(sql,"INSERT INTO users (name,age,telephone,address,username) VALUES ('','','%d', '', '%s');",num,me->user);

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, ANSI_COLOR_RED "username %s existe déjà\n" ANSI_COLOR_RESET , me->user);
      sqlite3_free(zErrMsg);
      isconnected=0;
   }else{
      fprintf(stdout, "Documents créés avec succès\n");
   
   sqlite3_close(db);
  
printf(ANSI_COLOR_YELLOW "prêt à envoyer et recevoir des messages \n" ANSI_COLOR_RESET);
sleep(1);
struct THREADINFO threadinfo;
pthread_create(&threadinfo.thread_ID, NULL, receiver, (void *)&threadinfo);
}
}
else {
fprintf(stderr, "Connection rejected...\n");
}

}

int connect_with_server() {
int newfd, err_ret;
struct sockaddr_in serv_addr;//pour communiquer via internet
struct hostent *to;//for host

/* generate address */
if((to = gethostbyname(SERVERIP))==NULL) {// gethostbyname: recieve info for host name
err_ret = errno;
fprintf(stderr, "gethostbyname() error...\n");
return err_ret;
}

/* open a socket */
if((newfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
err_ret = errno;
fprintf(stderr, "socket() error...\n");
return err_ret;
}

/* set initial values */
serv_addr.sin_family = AF_INET;
serv_addr.sin_port = htons(SERVERPORT);
serv_addr.sin_addr = *((struct in_addr *)to->h_addr);
memset(&(serv_addr.sin_zero), 0, 8);

/* try to connect with server */
if(connect(newfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) {
err_ret = errno;
fprintf(stderr, "connect() error...\n");
return err_ret;
}
else {
printf("Connecté au serveur au %s:%d\n", SERVERIP, SERVERPORT);
return newfd;
}
}

void logout(struct USER *me) {
	sqlite3 *db;
    char *err_msg = 0;
    sqlite3_stmt *res;
    int rc = sqlite3_open("chat.db", &db);
    
     if (rc == SQLITE_OK) {
    
     	
 const char *sql = "delete from users where username = ?";
    if(sqlite3_prepare_v2(db, sql, -1, &res, NULL) != SQLITE_OK)
       printf("Error while creating update statement. %s", sqlite3_errmsg(db));
}
rc=sqlite3_bind_text(res, 1,me->user,-1,0);



char* errmsg;
sqlite3_exec(db, "COMMIT", NULL, NULL, &errmsg);

if(SQLITE_DONE != sqlite3_step(res))
    printf("Error while deleting. %s", sqlite3_errmsg(db));
sqlite3_finalize(res);


sqlite3_close(db);
int sent;
struct PACKET packet;
if(!isconnected) {
fprintf(stderr, "You are not connected...\n");
return;
}
memset(&packet, 0, sizeof(struct PACKET));
strcpy(packet.option, "exit");
strcpy(packet.user, me->user);
/* send request to close this connetion */
sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
isconnected = 0;
}

void setuser(struct USER *me) {
int sent;
struct PACKET packet;
if(!isconnected) {
fprintf(stderr, "You are not connected...\n");
return;
}
memset(&packet, 0, sizeof(struct PACKET));
strcpy(packet.option, "change");
strcpy(packet.user, me->user);
/* send request to close this connetion */
sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);//send packet
}

void *receiver(void *param) {

int recvd;
struct PACKET packet;
printf("En attente de messages d'autres clients...\n");
while(isconnected) {
recvd = recv(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
if(!recvd) {
fprintf(stderr, ANSI_COLOR_RED "Connexion perdue du serveur...\n" ANSI_COLOR_RESET);
isconnected = 0;
close(sockfd);
break;
}
if(recvd > 0) {

if(!strcmp(packet.option, "msg")){
printf(ANSI_COLOR_YELLOW "diffuser à partir [%s]: %s \n" ANSI_COLOR_RESET, packet.user, packet.buff);
}
if(!strcmp(packet.option, "specf")){
printf(ANSI_COLOR_BLUE "message de[%s]: %s \n" ANSI_COLOR_RESET, packet.user, packet.buff);
}
if(!strcmp(packet.option, "encrypt")){
printf(ANSI_COLOR_CYAN "message chiffré à partir de [%s]: %s \n" ANSI_COLOR_RESET, packet.user, packet.buff);
}
if(!strcmp(packet.option, "sfile")){

printf(ANSI_COLOR_GREEN "reçu fichier à partir de [%s]: contenu: %s nom du fichier: %s \n" ANSI_COLOR_RESET, packet.user, packet.buff,packet.name);

FILE *f = fopen(packet.name, "a");

    fprintf(f,"%s\n", packet.buff);
fclose(f);


}
}
memset(&packet, 0, sizeof(struct PACKET));
}
return NULL;
}
char revbuf[LENGTH];

void sendtoall(struct USER *me, char *msg) {
int sent;
struct PACKET packet;

if(!isconnected) {
fprintf(stderr, "Vous n'êtes pas connecté...\n");
return;
}
msg[BUFFSIZE] = 0;
memset(&packet, 0, sizeof(struct PACKET));
strcpy(packet.option, "send");
strcpy(packet.user, me->user);
strcpy(packet.buff, msg);
/* send request to close this connetion */
sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}

void sendtoclient(struct USER *me, char *target, char *msg) {
int sent, targetlen;
struct PACKET packet;
if(target == NULL) {
return;
}
if(msg == NULL) {
return;
}
if(!isconnected) {
fprintf(stderr, "Vous n'êtes pas connecté...\n");
return;
}
msg[BUFFSIZE] = 0;
targetlen = strlen(target);
memset(&packet, 0, sizeof(struct PACKET));
strcpy(packet.option, "specf");
strcpy(packet.user, me->user);
strcpy(packet.buff, target);
strcpy(&packet.buff[targetlen], " ");
strcpy(&packet.buff[targetlen+1], msg);
/* send request to close this connetion */
sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}
void sendfile(struct USER *me, char *target, char *msg,char *buff) {
int sent, targetlen;
struct PACKET packet;

buff="asdS";
if(target == NULL) {
return;
}
if(msg == NULL) {
return;
}
if(!isconnected) {
fprintf(stderr, "Vous n'êtes pas connecté...\n");
return;
}
msg[BUFFSIZE] = 0;
targetlen = strlen(target);
memset(&packet, 0, sizeof(struct PACKET));
strcpy(packet.option, "sfile");
strcpy(packet.user, me->user);
strcpy(packet.buff, target);
strcpy(packet.name, msg);
strcpy(&packet.buff[targetlen], " ");
strcpy(&packet.buff[targetlen+1], buff);
/* send request to close this connetion */
sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}
void sendencrypt(struct USER *me, char *target, char *msg) {
int sent, targetlen;
struct PACKET packet;
if(target == NULL) {
return;
}
if(msg == NULL) {
return;
}
if(!isconnected) {
fprintf(stderr, "Vous n'êtes pas connecté...\n");
return;
}
int a=1;

char *c=(char*)&a;
printf(ANSI_COLOR_RED "message chiffré : %s\n" ANSI_COLOR_RESET,c);
msg[BUFFSIZE] = 0;
targetlen = strlen(target);
memset(&packet, 0, sizeof(struct PACKET));
strcpy(packet.option, "encrypt");
strcpy(packet.user, me->user);
strcpy(packet.buff, target);
strcpy(&packet.buff[targetlen], " ");
strcpy(&packet.buff[targetlen+1],c);
/* send request to close this connetion */
sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}
void profile(struct USER *me, char *msg){
	printf("le profile de %s\n",msg );
	 sqlite3 *db;
    char *err_msg = 0;
    sqlite3_stmt *res;
    
    int rc = sqlite3_open("chat.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return;
    }
    
    char *sql = "SELECT * FROM users WHERE telephone = ?";
        
    rc = sqlite3_prepare_v2(db, sql, -1, &res, NULL);
    
    if (rc == SQLITE_OK) {
       int telephone=atoi(msg);
       sqlite3_bind_int(res, 1, telephone);
    } else {
        
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    
    int step = sqlite3_step(res);
    
    if (step == SQLITE_ROW) {
        printf("name------>: %s\n", sqlite3_column_text(res, 4));
         printf("age------>: %s\n", sqlite3_column_text(res, 1));
          printf("telephone------->: %s\n", sqlite3_column_text(res, 2));
           printf("address------->: %s\n", sqlite3_column_text(res, 3));
        
    } 

    sqlite3_finalize(res);
    sqlite3_close(db);
    
    return;
}
void update_name(struct USER *me,char *target,char *msg){
 sqlite3 *db;
    char *err_msg = 0;
    sqlite3_stmt *res;
    int rc = sqlite3_open("chat.db", &db);
    
     if (rc == SQLITE_OK) {
    
     	
 const char *sql = "update users Set age = ? Where telephone = ?";
    if(sqlite3_prepare_v2(db, sql, -1, &res, NULL) != SQLITE_OK)
       printf("Error while creating update statement. %s", sqlite3_errmsg(db));
}
int age=atoi(msg);
sqlite3_bind_int(res, 1,age);

sqlite3_bind_int(res, 2 , num);


char* errmsg;
sqlite3_exec(db, "COMMIT", NULL, NULL, &errmsg);

if(SQLITE_DONE != sqlite3_step(res))
    printf("Error while updating. %s", sqlite3_errmsg(db));
sqlite3_finalize(res);


sqlite3_close(db);
	printf("change age: %s\n", msg);
}

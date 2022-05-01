#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/signal.h>
#include <arpa/inet.h>


#define PORT 4445
#define SIZE 1024

//create file on the server and receive the data for the file from the client
void write_file(int sockfd, char * fname)
{
    int n;
    FILE *fp;
    char *filename = fname ;
    char buffer[SIZE];

    fp = fopen(filename, "w");
    if(fp==NULL)
    {
        perror("[-]Error in creating file.");
        exit(1);
    }
    int totalReceived = 0 ;
    while(1)
    {


        n = recv(sockfd, buffer, SIZE, 0);
        totalReceived = totalReceived + n ;
        if(n<=0 || strstr(buffer, "EOF Close Now") )
        {
            fprintf(stderr, "Full file recived\n");
            break;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    fclose(fp) ;

    return;

}

//grep the file with the searched word to get the total number of lines
void grepTheFileNumber(int sd, char *searchWord,char *filename){
    printf("apple") ;
    sleep(2) ;
    char grepCMD[SIZE];
    snprintf(grepCMD, sizeof(grepCMD), "%s%s%s%s", "grep -w ", searchWord, " ", filename);

    FILE *cmd=popen(grepCMD, "r");
    char result[1024]={0x0};
    int i = 0 ;
    while (fgets(result, sizeof(result), cmd) !=NULL){
        fprintf(stderr, "%s: %s", filename,  result);
        i = i + 1 ;
    }

    // convert 123 to string [buf]
    char snum[5];
    int n = sprintf( snum, "%d", i);
    write(sd, snum , strlen(snum)+1);
    pclose(cmd);
}

//grep the file with the searched word
void grepTheFile(int sd, char *searchWord,char *filename){

    char grepCMD[SIZE];
    snprintf(grepCMD, sizeof(grepCMD), "%s%s%s%s", "grep -w ", searchWord, " ", filename);

    FILE *cmd=popen(grepCMD, "r");
    char result[1024]={0x0};
    int i = 0 ;
    while (fgets(result, sizeof(result), cmd) !=NULL){
        write(sd, result , strlen(result)+1);
        sleep(1) ;
    }

    pclose(cmd);
}


//handle each forked client
void handle_client(int sd){
    char filename[256];
    char searchWord[256];

    //read search word from client
    if(!read(sd, searchWord, 256)){
        close(sd);
        fprintf(stderr,"Client is dead, wait for a new client\n");
        exit(0);
    }
    fprintf(stderr, "Client sent back this message: %s\n", searchWord);

    //read file name from client
    if(!read(sd, filename, 256)){
        close(sd);
        fprintf(stderr,"Client is dead, wait for a new client\n");
        exit(0);
    }
    fprintf(stderr, "Client sent back this message: %s\n", filename);

    sleep(3) ;

    //create fork for the creating the file
    if (fork() == 0){
        write_file(sd, filename) ;
        exit(0) ;

    } else{
        wait(NULL) ;
        sleep(2) ;

        //fork to get the total number of results from the file
        if (fork() == 0){
            grepTheFileNumber(sd,searchWord,filename);

            exit(0) ;
        } else{
            wait(NULL) ;
            //grep the file and send back data
            grepTheFile(sd,searchWord,filename);
            fprintf(stderr,"msg sent\n");
        }

        fprintf(stderr,"done msg final\n");
    }

}

int main(){
    int sd, csd, portNumber;
    socklen_t len;
    struct sockaddr_in servAdd;

    portNumber = 4457 ;
//create socket
    if ((sd=socket(AF_INET,SOCK_STREAM,0))<0){
        fprintf(stderr, "Cannot create socket\n");
        exit(1);
    }

    servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdd.sin_port = htons((uint16_t)portNumber);

//bind the socket
    bind(sd,(struct sockaddr*)&servAdd,sizeof(servAdd));

//listen the client
    listen(sd, 5);

    while(1){
        //accept new client
        csd=accept(sd,(struct sockaddr*)NULL,NULL);
        printf("Got a client\n");
        sleep(2);
        if(fork()==0)
            //call this function for every client
            handle_client(csd);
        close(csd);
    }

    return 0 ;
}


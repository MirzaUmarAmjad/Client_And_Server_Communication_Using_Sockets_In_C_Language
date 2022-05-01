//// Client Side program to test
//// the TCP server that returns
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include<fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>


#define SIZE 1024

//return the total number of character or file size
long int findSize(char file_name[])
{
    // opening the file in read mode
    FILE* fp = fopen(file_name, "r");

    // checking if the file exist or not
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file
    long int res = ftell(fp);

    // closing the file
    fclose(fp);

    return res;
}

//send file to server
void send_file(FILE *fp, int sockfd,char *fileName)
{
    struct stat st;
    char text[20];
    stat(fileName, &st);
    int size = st.st_size;
    sprintf(text, "%d", size);
    printf("%d", size ) ;

    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp)!=NULL)
    {
        printf(data) ;
        if(send(sockfd, data, sizeof(data), 0)== -1)
        {
            perror("[-] Error in sending data");
            exit(1);
        }
        bzero(data, SIZE);
    }
    send(sockfd, "EOF Close Now", sizeof("EOF Close Now") + 1 , 0);
    bzero(data, SIZE);

}

//get filname from the path given by the user
char* getFileName(char * filePath){
    char* local_file = filePath;

    char* ts1 = strdup(local_file);
    char* ts2 = strdup(local_file);

    char* dir = dirname(ts1);
    char* filename = basename(ts2);

    return filename;
}

//find the search keyword in a text file using grep
void grepTheFile(char *searchWord,char *filename){

    char grepCMD[SIZE];
    snprintf(grepCMD, sizeof(grepCMD), "%s%s%s%s", "grep -w ", searchWord, " ", filename);

    FILE *cmd=popen(grepCMD, "r");
    char result[1024]={0x0};
    int i = 0 ;
    while (fgets(result, sizeof(result), cmd) !=NULL){
//        fprintf(stderr, "%s: %s", filename,  result);



        const char s[2] = " ";
        char *token;

        /* get the first token */
        token = strtok(result, s);
//                    if (strstr(token, "\n") != NULL) {
        //send file name to server
        char* fName2 = getFileName(filename) ;
        printf( "%s: ", fName2 ); //printing each token
//                    }
        /* walk through other tokens */
        while( token != NULL ) {


            if (strcmp(token,searchWord) == 0){
                printf( "\033[31;1m %s\033[0m", token ); //printing each token

            }
            else{
                printf( " %s", token ); //printing each token
            }

            token = strtok(NULL, s);
        }



    }

    pclose(cmd);
}


int main(int argc, char *argv[]){
    char message[255];
    int ssd, portNumber;
    portNumber = 4457 ;
    char * ip = "127.0.0.1" ;
    char *filename = argv[3];
    FILE *fp;
    FILE *file;

    socklen_t len;
    struct sockaddr_in servAdd;

    //check if user pass all three arguments
    if (argc != 4){
        fprintf(stderr, "Kindly pass all arguments\n");
        exit(1);
    }

    //check file 1 is available
    if((file = fopen(argv[2],"r"))!=NULL)
    {
        // file exists
        fclose(file);
    } else{
        fprintf(stderr, "File 1 does not exist\n");
        exit(1);
    }

    //check file 2 is available
    if((file = fopen(argv[3],"r"))!=NULL)
    {
        // file exists
        fclose(file);
    } else{
        fprintf(stderr, "File 2 does not exist\n");
        exit(1);
    }

//create socket
    if((ssd = socket(AF_INET, SOCK_STREAM, 0))<0){
        fprintf(stderr, "Cannot create socket\n");
        exit(1);
    }

    servAdd.sin_family = AF_INET;
    servAdd.sin_port = htons((uint16_t)portNumber);//convert host bytestream into network bytestream

    if(inet_pton(AF_INET,ip,&servAdd.sin_addr)<0){
        fprintf(stderr, " inet_pton() has failed\n");
        exit(2);
    }

//connect with the server
    if(connect(ssd,(struct sockaddr *)&servAdd,sizeof(servAdd))<0){
        fprintf(stderr, "connect() has failed, exiting\n");
        exit(3);
    }

    //send search word to server
    write(ssd, argv[1], strlen(argv[1])+1);

    //send file to server
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        perror("[-]Error in reading file.");
        exit(1);
    }
    sleep(3) ;


    if (fork() == 0){
        //send file name to server
        char* fName = getFileName(argv[3]) ;
        write(ssd, fName, strlen(fName)+1);
        sleep(3) ;
        send_file(fp,ssd,fName);

        exit(0) ;

    }
    else{
        wait(NULL) ;
        //receive the serached word from the server
        fprintf(stderr,"file sent\n");
        char message2266[255];
        if(read(ssd, message2266, 255)<0){
            fprintf(stderr, "read() error\n");
            exit(3);
        }
        fprintf(stderr, "server's message: %s\n",message2266);




        //file 1 start
        grepTheFile(argv[1],argv[2]) ;
        //file 2 start





                //tokenize the received data
                int x = atoi(message2266);
                char message242342[SIZE];
                for( int a = 0; a < x; a = a + 1 ) {

                    if(read(ssd, message242342, SIZE)<0){
                        fprintf(stderr, "read() error\n");
                        exit(3);
                    }

                    const char s[2] = " ";
                    char *token;

                    /* get the first token */
                    token = strtok(message242342, s);
//                    if (strstr(token, "\n") != NULL) {
                        //send file name to server
                        char* fName2 = getFileName(argv[3]) ;
                        printf( "%s: ", fName2 ); //printing each token
//                    }
                    /* walk through other tokens */
                    while( token != NULL ) {


                        if (strcmp(token,argv[1]) == 0){
                            printf( "\033[31;1m %s\033[0m", token ); //printing each token

                        }
                        else{
                            printf( " %s", token ); //printing each token
                        }

                        token = strtok(NULL, s);
                    }




                }
        }

}
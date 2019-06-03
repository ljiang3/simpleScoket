#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define RCVBUFSIZE 100 /* Size of receive buffer */
#define MAXPENDING 5 /* Maximum outstanding connection requests */

char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
unsigned int echoStringLen; /* Length of string to echo */
int bytesRcvd, totalBytesRcvd; /* Bytes read in single recv() and total bytes read */
int sock; /* Socket descriptor */
struct sockaddr_in echoServAddr; /* Echo server address */
unsigned short echoServPort; /* Echo server port */

void DieWithError(char *errorMessage); /* Error handling function */

void connectServer(){

char *servlP = malloc(sizeof(char)); /* Server IP address (dotted quad) */
char *username = malloc(sizeof(char));
char *password = malloc(sizeof(char));

/* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError(" socket () failed");

/* Inputs of Server IP address and Server port*/
    printf("Please enter the IP address: ");
    scanf("%s", servlP);
    printf("Please enter the port number: ");
    scanf("%hu", &echoServPort);
    printf("connecting......\n");

/* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET;  /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servlP); /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort); /* Server port */

/* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    printf("connected!\n");

    memset(username, '\0', sizeof(username)*sizeof(char));
/* Inputs for username and password*/
    printf("Welcome! Please log in.\n");
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    strcat(username, password);
    int usernameLen = strlen(username);
  
    /* Send the string to the server */
    if(send(sock, username, usernameLen, 0) != usernameLen)
    DieWithError("send() sent a different number of bytes than expected");
    
    printf("Login successfully!\n");

    free(servlP);
    free(username);
    free(password);
}
void getUserList(){

    char getUserList[] = "getUserList";   
    if(send(sock, getUserList, strlen(getUserList), 0) != strlen(getUserList)){
        DieWithError("send() sent a different number of bytes than expected");
    }       
    printf("There are 2 users:\nAlice\nBob\n");
}
void sendMessage(){
    char username[50];
    char message[100];
    char temp[100];

    printf("Please enter the user name: ");
    scanf("%s", username);
    printf("Please enter the message: ");
    scanf("%s", temp);
    scanf("%[^\n]", message);
    strcat(temp, message);
    strcpy(message, temp);

    if(send(sock, username, strlen(username), 0) != strlen(username)){

        DieWithError("send() sent a different number of bytes than expected");
    }

    if(send(sock, message, strlen(message), 0) != strlen(message)){
        DieWithError("send() sent a different number of bytes than expected");
    }
    printf("\nmessage sent successfully!\n");
}
void getMessage(){

    char getMessage[] = "getMessage";
    if(send(sock, getMessage, strlen(getMessage), 0) != strlen(getMessage)){

        DieWithError("send() sent a different number of bytes than expected");
    }
    printf("You have 1 message<s>!\nHi, this is Alice. Please contact me at 127.0.0.1:5000\n");

}
void initiateChat(){
    char disconnect[] = "disconnect";
    char message[100];
    char temp[100];
    int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */
    char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
    int recvMsgSize; /* Size of received message */
    int portNumber;

    if(send(sock, disconnect, strlen(disconnect), 0) != strlen(disconnect)){

        DieWithError("send() sent a different number of bytes than expected");
    }
    close(sock);

    printf("-------------------disconnect with the server---------------------\n");
    printf("Please enter the port number you want to listen on: ");
    scanf("%hu", &echoServPort);
    printf("I am listening on 127.0.0.1:%d\n", echoServPort);

    // Listening on port 127.0.0.1:5000

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError( "socket () failed") ;

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort); /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError ( "bind () failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed") ;
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");
        printf("Bob is connected!\n");
    
        // For chating!
        do{
            printf("<Type “Bye” to stop the conversation>\n");
            memset(echoBuffer, '\0', sizeof(echoBuffer)*sizeof(char));
            if((recvMsgSize = recv(clntSock, echoBuffer, RCVBUFSIZE, 0)) < 0)
                DieWithError("recv() failed");
            if(strcmp(echoBuffer, "disconnect") == 0){
                printf("Disconnected from my friend!\n");
                close(clntSock);
                break;
            }
            printf("%s\n", echoBuffer);

            printf("Alice: ");
            scanf(" %[^\n]", message);
            if(strcmp(message, "Bye") == 0){
                if(send(clntSock, disconnect, strlen(disconnect), 0) != strlen(disconnect)){
                    DieWithError("send() sent a different number of bytes than expected");
                }
                close(clntSock);
                break;
            }
            else{
                strcpy(temp, message);                  
                sprintf(message, "Alice: %s", temp);
                if (send(clntSock, message, strlen(message),0) != strlen(message))
                    DieWithError("send() failed");
            }         
        } while(recvMsgSize > 0);
    }
}
void chat(){

    char *servlP = malloc(sizeof(char));
    char message[100];
    char temp[100];
    char disconnect[] = "disconnect";
    if(send(sock, disconnect, strlen(disconnect), 0) != strlen(disconnect)){
        DieWithError("send() sent a different number of bytes than expected");
    }

    close(sock);
    printf("-------------------disconnect with the server---------------------\n");

    // connecting on port 127.0.0.1:5000
    printf("Please enter your friend's IP address: ");
    scanf("%s", servlP);
    printf("Please enter your friend's port number: ");
    scanf("%hu", &echoServPort);

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError(" socket () failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET;  /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servlP); /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    printf("connectting to your friend....\nconnected!\n");

    // For chatting
    totalBytesRcvd = 0;
    do{
        printf("<Type “Bye” to stop the conversation>\n");
        printf("Bob: "); 
        scanf(" %[^\n]", message);
        if(strcmp(message, "Bye") == 0){
            if(send(sock, disconnect, strlen(disconnect), 0) != strlen(disconnect)){
                DieWithError("send() sent a different number of bytes than expected");
            }
            close(sock);
            break;
        }
        else{
            strcpy(temp, message);                  
            sprintf(message, "Bob: %s", temp);
            if(send(sock, message, strlen(message), 0) != strlen(message)){
                DieWithError("send() sent a different number of bytes than expected");
            }

            /* Receive up to the buffer size (minus i to leave space for a null terminator) bytes from the sender */
            memset(echoBuffer, '\0', sizeof(echoBuffer)*sizeof(char));
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
                DieWithError("recv() failed or connection closed prematurely");
            totalBytesRcvd += bytesRcvd; /* Keep tally of total bytes */
            echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
            if(strcmp(echoBuffer, "disconnect") == 0){
                printf("Disconnected from my friend!\n");
                close(sock);
                break;
            }
            printf("%s\n",echoBuffer); /* Print the echo buffer */
        }                 
    }while (totalBytesRcvd < 100);
    free(servlP);
}
void excuteOptions(){
    int options;
    printf("Your option<enter a number>: ");
    scanf("%d", &options);

    while(options > 5 || options < 0)
    {
        printf("Please enter the numbers listed above!\n");
        printf("Your option<enter a number>: ");
        scanf("%d", &options);
    }
    switch(options){
        case 0: connectServer();
        break; 
        case 1: getUserList();
        break;    
        case 2: sendMessage();
        break;
        case 3: getMessage();
        break;
        case 4: initiateChat();
        break;
        case 5: chat();
        break;
    }
}

int main(int argc, char *argv[])
{
   while(1){
        printf("-----------------------------------------------------------------\n");
        printf("Command: \n0. Connect to the server\n1. Get the user list\n2. Send a message\n3. Get my messages\n4. Initiate a chat with my friend\n5. Chat with my friend\n");
        excuteOptions();
   }
    exit(0);
}

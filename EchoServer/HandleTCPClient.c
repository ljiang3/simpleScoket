#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h> /* for close() */
#include <stdlib.h>
#include <string.h>

#define RCVBUFSIZE 100  /* Size of receive buffer */

void DieWithError(char *errorMessage); /* Error handling function */

void HandleTCPClient(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
    int recvMsgSize; /* Size of received message */ 

    do{
        memset(echoBuffer, '\0', sizeof(echoBuffer)*sizeof(char));
  
         /* Receive message from client */
        if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            DieWithError("recv() failed"); 

        if(strcmp(echoBuffer, "Bob56789") == 0){
            printf("Login username is Bob.\nLogin password is 56789\n");
        }

        else if(strcmp(echoBuffer, "Alice12345") == 0){
            printf("Login username is Alice.\nLogin password is 12345\n");
        }

        else if(strcmp(echoBuffer, "getUserList") == 0){
            printf("Return user list!\n");
        }

        else if(strcmp(echoBuffer, "Bob") == 0){
            printf("A message to %s\n", echoBuffer);
        }

        else if(strcmp(echoBuffer, "Alice") == 0){
            printf("A message to %s\n", echoBuffer);
        }

        else if(strcmp(echoBuffer, "getMessage") == 0){
            printf("Send back Bob's message!\n");
        }

        else if(strcmp(echoBuffer, "disconnect") == 0){
            printf("Client disconnected!\n");
        }

        else if(strcmp(echoBuffer, "Hi, this is Alice. Please contact me at 127.0.0.1:5000") == 0){
            printf("Message is %s\n", echoBuffer);
        }

    }while(recvMsgSize > 0); /* zero indicates end of transmission */

    close(clntSocket); /* Close client socket */

}

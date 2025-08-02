#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 1337
#define IP_ADDR "127.0.0.1" // localhost

int main() {

  int clientFd;
  char recvBuffer[256]; // receive server message
  char sendBuffer[256]; // send server message
  struct sockaddr_in serverAddr;

  memset(recvBuffer, 0, sizeof(recvBuffer)); // set all values to 0
  memset(sendBuffer, 0, sizeof(sendBuffer)); // this too

  // initialize the socket
  clientFd = socket(AF_INET, SOCK_STREAM, 0);

  // declare the server information
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr(IP_ADDR);

  // connect the socket with the server
  if (connect(clientFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    perror("Connect failed");
    return EXIT_FAILURE;
  }
  while (1) {
    memset(recvBuffer, 0, sizeof(recvBuffer));
    printf("Message : ");
    fgets(sendBuffer, 256, stdin);
    send(clientFd, sendBuffer, strlen(sendBuffer), 0); // sends to server
    recv(clientFd, recvBuffer, sizeof(recvBuffer), 0); // receive from server
    printf("Server : %s", recvBuffer);
  }

  close(clientFd);
  return EXIT_SUCCESS;
}

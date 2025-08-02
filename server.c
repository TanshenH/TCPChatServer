#include <arpa/inet.h> // inet functions
#include <asm-generic/socket.h>
#include <netinet/in.h> // struct sockaddr_in struture
#include <stdio.h>      // logging
#include <stdlib.h>     // system related func
#include <string.h>     // mem related funcs
#include <sys/socket.h> // socket
#include <sys/types.h>  // for more datatypes
#include <unistd.h>     // close()

#define PORT 1337 // port 1337 , hehehe elite coder !
#define MAX_CLIENT 10

int main() {

  char recvBuffer[256]; // receive from clients
  char sendBuffer[256]; // send to clients
  int serverFd;
  int clientFd;
  int clientNum = 0;
  int checkRecv;
  int opt = 1;
  struct sockaddr_in serverAddr;
  struct sockaddr clientAddr;
  socklen_t clientAddr_len = sizeof(clientAddr);

  memset(&recvBuffer, 0, 256); // set all values to 0
  memset(&sendBuffer, 0, 256); // this too

  // initialize the server socket : like make it socket
  serverFd = socket(AF_INET, SOCK_STREAM, 0);

  // set option to reuse address
  setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // declare the server socket information
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("Bind failed");
    return EXIT_FAILURE;
  }

  printf("Number of client connected : %d\n", clientNum);
  while (1) {
    // wait for clients
    listen(serverFd, MAX_CLIENT);
    clientFd = accept(serverFd, &clientAddr,
                      &clientAddr_len); // accept the incoming connection
    if (clientFd < 0) {
      perror("accept failed");
      continue;
    }
    clientNum++;
    printf("Number of client connected : %d\n", clientNum);

    while (1) {
      memset(recvBuffer, 0, 256);
      // receive message from client
      checkRecv = recv(clientFd, recvBuffer, sizeof(recvBuffer), 0);
      if (checkRecv == 0) {
        printf("Client Disconnected\n");
        break;
      } else if (checkRecv < 0) {
        perror("Recv failed");
        break;
      }

      printf("Client%d : %s", clientNum, recvBuffer);
      printf("Message : ");
      fgets(sendBuffer, 256, stdin);
      send(clientFd, sendBuffer, strlen(sendBuffer),
           0); // send message to client
    }
    clientNum--;
    close(clientFd);
    printf("\nNumber of client connected : %d\n", clientNum);
  }
  close(serverFd);
  return EXIT_SUCCESS;
}

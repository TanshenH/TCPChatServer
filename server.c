/*
 *  Multithreaded server
 *  written by : github.com/TanshenH
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 1337
#define MAX_CLIENTS 50
#define MAX_BUFFER 1024

typedef struct Client {
  int fd;
  int id;
} Client;

void broadcastMessage(const char *message, int clientSocket);
void *handleClient(void *arg);

Client ClientsConnected[MAX_CLIENTS];
pthread_mutex_t clientMutex = PTHREAD_MUTEX_INITIALIZER;
int clientCount = 0;

int main() {

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  int sockOpt = 1;
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(sockOpt));

  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    perror("Bind failed");
    return 0;
  }

  listen(serverSocket, MAX_CLIENTS);

  printf("Server started on PORT %d\n", PORT);
  while (1) {
    int clientSocket = accept(serverSocket, NULL, NULL);

    pthread_mutex_lock(&clientMutex);
    if (clientCount >= MAX_CLIENTS) {
      close(clientSocket);
      pthread_mutex_unlock(&clientMutex);
      continue;
    }

    Client *client = malloc(sizeof(Client));
    client->fd = clientSocket;
    client->id = clientCount + 1;

    ClientsConnected[clientCount++] = *client;
    printf("Client %d connected\n", client->id);
    pthread_mutex_unlock(&clientMutex);

    pthread_t clientThread;
    pthread_create(&clientThread, NULL, handleClient, client);
    pthread_detach(clientThread);
  }

  close(serverSocket);
  return 0;
}

void broadcastMessage(const char *message, int clientId) {

  pthread_mutex_lock(&clientMutex);
  for (int i = 0; i < clientCount; i++) {
    if (ClientsConnected[i].id != clientId) {
      write(ClientsConnected[i].fd, message, strlen(message));
    }
  }
  pthread_mutex_unlock(&clientMutex);
}

void *handleClient(void *arg) {

  Client *client = (Client *)arg;
  char buffer[MAX_BUFFER] = {0};

  char welcome[50] = {0};
  char currentClients[50] = {0};
  snprintf(welcome, sizeof(welcome),
           "Welcome To the Chat Server ! Your ID is : %d\n", client->id);
  snprintf(currentClients, sizeof(currentClients), "Connected Clients : %d\n",
           clientCount);
  write(client->fd, welcome, sizeof(welcome));
  write(client->fd, currentClients, sizeof(currentClients));

  while (1) {
    int bytesRead = read(client->fd, buffer, sizeof(buffer));
    if (bytesRead <= 0) {

      pthread_mutex_lock(&clientMutex);
      printf("Client %d Disconnected\n", client->id);

      // search the clientid in connected client array and reduce the clientid
      for (int i = 0; i < clientCount; i++) {
        if (ClientsConnected[i].id == client->id) {
          for (int j = i; j < clientCount - 1; j++) {
            ClientsConnected[j] = ClientsConnected[j + 1];
          }
          clientCount--;
          break;
        }
      }

      pthread_mutex_unlock(&clientMutex);
      close(client->fd);
      free(client);
      return NULL;
    }

    buffer[bytesRead] = '\0';
    printf("Client %d: %s", client->id, buffer);

    char message[MAX_BUFFER];
    snprintf(message, sizeof(message), "Client %d: %s", client->id, buffer);

    broadcastMessage(message, client->id);
  }
}

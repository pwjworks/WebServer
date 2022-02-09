//
// Created by pwjworks on 2022/2/9.
//
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

void error_handling(const std::string &message) {
  std::cout << message << std::endl;
  exit(1);
}

int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in serv_adr {};
  char message[100];
  if (argc != 3) {
    printf("Usage %s <IP> <port>\n", argv[0]);
    exit(1);
  }
  // 创建socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
    error_handling("socket() error");

  memset(&serv_adr, 0, sizeof serv_adr);
  serv_adr.sin_family = AF_INET;
  serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_adr.sin_port = htons(atoi(argv[2]));

  if (connect(sock, (struct sockaddr *) &serv_adr, sizeof serv_adr) == -1)
    error_handling("connect() error!");
  else
    puts("Connected.........");

  while (true) {
    fputs("Input message(Q to quit):\n", stdout);
    fgets(message, 100, stdin);

    if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
      break;
    send(sock, message, strlen(message), 0);
    ssize_t str_len = recv(sock, message, 100, 0);
    printf("recv size:%zu\n", strlen(message));
    if (str_len == -1)
      break;
    message[str_len] = 0;
    printf("Message from server: %s", message);
  }
  close(sock);
  return 0;
}
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

int32_t main() {

  // SO_KEEPALIVE -> SIGPIPE
  // SO_REUSEADDR
  int listen_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
  if (listen_socket == -1) {
      perror("Socket can't be created\n");
      return -1;
  }

  int ret_val = 0;

/*
  struct sockaddr_in {
      sa_family_t   sin_family; // address family: AF_INET
      in_port_t     sin_port;   //  port in network byte order
      struct in_addr sin_addr;  // internet address
  };

  struct in_addr {
      uint32_t s_addr; // address in network byte order
  };
*/

  struct sockaddr_in listen_socket_addr;
  listen_socket_addr.sin_family = AF_INET;
  listen_socket_addr.sin_port = htons(10002);
  listen_socket_addr.sin_addr.s_addr = INADDR_ANY;
  //listen_socket_addr.sin_addr.s_addr = inet_addr("10.20.30.2");

  socklen_t addrlen = sizeof(listen_socket_addr);
  // man 7 ip
  ret_val = bind(listen_socket, (const struct sockaddr *restrict)&listen_socket_addr, sizeof(listen_socket_addr));
  if (ret_val == -1) {
      perror("Binding can't be carried out\n");
      return -2;
  }

  ret_val = listen(listen_socket, 1);
  if (ret_val == -1) {

      perror("Listening can't be carried out\n");
      close(listen_socket);
      return -3;
  }

  struct sockaddr_in client_addr;
  socklen_t client_addr_len;
  int communication_socket;
  while(1) {
      communication_socket = accept(listen_socket, (struct sockaddr *restrict)&client_addr, &client_addr_len);
      if (communication_socket == -1) {
          if (errno == EAGAIN) {
              sleep(1);
              continue;
          }
          perror("Accepting can't be carried out\n");
          close(listen_socket);
          return -4;
      }
      break;
  }

  uint8_t buffer[16];
  while (1) {
      size_t len = sizeof(buffer);

      ssize_t recv_val = recv(communication_socket, buffer, len, 0);
      if (recv_val == -1) {
          if (errno == EAGAIN) {
              sleep(1);
              continue;
          }
          perror("Receiving can't be carried out\n");
          close(listen_socket);
          close(communication_socket);
          return -5;
      }
      if (recv_val == 0) {
          printf("Connection was closed\n");
          sleep(1);
          continue;
      }
      // TODO Process data
  }

//  send();

  close(communication_socket);
  close(listen_socket);

  return 0;
}

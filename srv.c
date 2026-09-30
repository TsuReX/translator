#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
/*
struct sigaction {
    void     (*sa_handler)(int);
    void     (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t   sa_mask;
    int        sa_flags;
    void     (*sa_restorer)(void);
};

int sigaction(int signum, const struct sigaction *_Nullable restrict act, struct sigaction *_Nullable restrict oldact);
*/

uint32_t shared_value = 0;
uint32_t exit_flag = 0;

static void srv_signal_handle(int32_t sig_num) {

	switch (sig_num) {
		case SIGINT:
			exit_flag = 1;
      printf("\nSignal was caught\n");
			break;
		default:
			exit(-7);
	}
}

int32_t main() {
  if (shared_value == 1) {
    printf("Dich hapened!!!\n");
    exit(-8);
  }
  printf("Shared value: %d \n", shared_value++);
  struct sigaction act;
	act.sa_handler = srv_signal_handle;
	if (sigaction(SIGINT, &act, NULL) == -1) {
		perror("Error of signal handlers setting up.");
		return -6;
	}
  printf("Program interrupt handler registered\n");
  // SO_KEEPALIVE -> SIGPIPE
  // SO_REUSEADDR
  int listen_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
  if (listen_socket == -1) {
      perror("Socket can't be created");
      return -1;
  }
  printf("Socket %d created\n", listen_socket);
  int ret_val = 0;

  struct sockaddr_in listen_socket_addr;
  listen_socket_addr.sin_family = AF_INET;
  listen_socket_addr.sin_port = htons(10002);
  listen_socket_addr.sin_addr.s_addr = INADDR_ANY;
  //listen_socket_addr.sin_addr.s_addr = inet_addr("10.20.30.2");

  // man 7 ip
  ret_val = bind(listen_socket, (const struct sockaddr *restrict)&listen_socket_addr, sizeof(listen_socket_addr));
  if (ret_val == -1) {
      perror("Binding can't be done");
      return -2;
  }
  printf("Socket was bountd with address\n");

  ret_val = listen(listen_socket, 1);
  if (ret_val == -1) {

      perror("Listening can't be started");
      close(listen_socket);
      return -3;
  }
  printf("Listening was started\n");

  struct sockaddr_in client_addr;
  socklen_t client_addr_len;
  int communication_socket;
  while(1) {
      if (exit_flag == 1) {
        printf("Program was interrupted\n");
        close(listen_socket);
//        exit(-7);
        return -7;
      }

      communication_socket = accept(listen_socket, (struct sockaddr *restrict)&client_addr, &client_addr_len);
      if (communication_socket == -1) {
          if (errno == EAGAIN) {
              sleep(1);
              printf("Connection request hasn't received\n");
              continue;
          }
          perror("Accepting can't be done");
          close(listen_socket);
          return -4;
      }


    printf("Connection %d was accepted\n", communication_socket);

    uint8_t buffer[16];
    size_t len = sizeof(buffer);
    uint32_t pos = 0;
    while (1) {
        if (exit_flag == 1) {
          printf("Program was interrupted\n");
          close(communication_socket);
          close(listen_socket);
  //        exit(-7);
          return -7;
        }

        ssize_t recv_val = recv(communication_socket, buffer + pos, len, MSG_DONTWAIT);
        if (recv_val == -1) {
            if (errno == EAGAIN) {
                printf("Data hasn't received\n");
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
     //       continue;
            break; // Start listening again
        }
        // Data processing
        len -= recv_val;
        pos += recv_val;
        if (pos >= 4) {
          if(strstr((char *)buffer, "\n") != NULL) {
            printf("Command: %s", buffer);
            len = sizeof(buffer);
            pos = 0;
          }
        }
        if (len == 0) {
//          printf("Buffer: %s\n", buffer);
          len = sizeof(buffer);
          pos = 0;
        }

    } // Receiving loop

    printf("Listening was started\n");
  } // Accepting loop
//  send();

  close(communication_socket);
  close(listen_socket);

  return 0;
}

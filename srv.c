#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "rbuff.h"
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

#define CMD_HEADER "RL"
#define CMD_TAIL "\n"
#define CMD_HEADER_LEN 2 // "RL"
#define CMD_MAX_LEN 8 // "RLATTBP\n

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

static void print_buffer(const uint8_t * buffer, size_t data_length) {
//  printf("Buffer: ");
  for (uint64_t i = 0; i < data_length; i++) {
    printf("0x%X ", buffer[i]);
  }
  printf("\n");
}

void fill_buffer(uint8_t * buffer, size_t data_length, uint8_t filler) {
  if ((buffer == NULL) || (data_length == 0))
    return;

  for (uint32_t i = 0; i < data_length; i++)
    buffer[i] = filler;
}

void rbuff_test() {
  struct rbuff_t rbuff;
  init_rbuff(&rbuff);
  uint8_t buffer0[0x20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0};
  uint8_t buffer1[0x20] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  size_t data_length0 = 0x10;
  size_t data_length1 = 0;
  size_t data_length0_before = data_length0;
  int32_t ret_val = 0;

  printf("sizeof(size_t): %ld\n",sizeof(size_t));
  /* copy_to_rbuff()  */
  printf("\n\n");
  printf("copy_to_rbuff() function testing\n");
  printf("buffer0: ");
  print_buffer(buffer0, sizeof(buffer0));

  ret_val = copy_to_rbuff(&rbuff, buffer0, &data_length0);
  printf("copy_to_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);

  data_length1 = 0x11;
  size_t data_length1_before = data_length1;

  debug_print_rbuff(&rbuff);

  /* copy_from_rbuff()  */
  printf("\n\n");
  printf("copy_from_rbuff() function testing\n");
  /* 1 */
  printf("**1**\n");
  printf("buffer1 before: ");
  print_buffer(buffer1, sizeof(buffer1));

  ret_val = copy_from_rbuff(&rbuff, buffer1, &data_length1, 0);
  printf("copy_from_rbuff(): %d, data_length1 before: %ld, data_length1 after %ld\n", ret_val, data_length1_before, data_length1);

  printf("buffer1 after: ");
  print_buffer(buffer1, sizeof(buffer1));
  debug_print_rbuff(&rbuff);
  /* 2 */
  printf("**2**\n");
  fill_buffer(buffer0, sizeof(buffer0), 0xA5);
  fill_buffer(buffer1, sizeof(buffer1), 0xFE);
  print_buffer(buffer0, sizeof(buffer0));
  print_buffer(buffer1, sizeof(buffer1));

  data_length0 = 8;
  data_length0_before = data_length0;
  ret_val = copy_from_rbuff(&rbuff, buffer0, &data_length0, 1);
  printf("copy_from_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);
  print_buffer(buffer0, sizeof(buffer0));
  debug_print_rbuff(&rbuff);

  data_length1 = 18;
  data_length1_before = data_length1;
  ret_val = copy_from_rbuff(&rbuff, buffer1, &data_length1, 1);
  printf("copy_from_rbuff(): %d, data_length1 before: %ld, data_length1 after %ld\n", ret_val, data_length1_before, data_length1);
  print_buffer(buffer1, sizeof(buffer1));
  debug_print_rbuff(&rbuff);
  /* 3 */
  printf("**3**\n");
  uint8_t buffer3[] = {1,2,3,4,5,6,7,8,9,0xA};
  printf("buffer3");
  print_buffer(buffer3, sizeof(buffer3));
  data_length0 = sizeof(buffer3);
  data_length0_before = data_length0;
  ret_val = copy_to_rbuff(&rbuff, buffer3, &data_length0);
  printf("copy_to_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = sizeof(buffer3);
  data_length0_before = data_length0;
  ret_val = copy_to_rbuff(&rbuff, buffer3, &data_length0);
  printf("copy_to_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = sizeof(buffer3);
  data_length0_before = data_length0;
  ret_val = copy_to_rbuff(&rbuff, buffer3, &data_length0);
  printf("copy_to_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = sizeof(buffer3);
  data_length0_before = data_length0;
  ret_val = copy_to_rbuff(&rbuff, buffer3, &data_length0);
  printf("copy_to_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = sizeof(buffer3);
  data_length0_before = data_length0;
  ret_val = copy_to_rbuff(&rbuff, buffer3, &data_length0);
  printf("copy_to_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = sizeof(buffer3);
  data_length0_before = data_length0;
  ret_val = copy_to_rbuff(&rbuff, buffer3, &data_length0);
  printf("copy_to_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = sizeof(buffer3);
  data_length0_before = data_length0;
  ret_val = copy_to_rbuff(&rbuff, buffer3, &data_length0);
  printf("copy_to_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = sizeof(buffer3);
  data_length0_before = data_length0;
  ret_val = copy_to_rbuff(&rbuff, buffer3, &data_length0);
  printf("copy_to_rbuff(): %d, data_length0 before: %ld, data_length0 after %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  /* find_in_rbuff() */
  printf("\n\n");
  printf("find_in_rbuff() function testing\n");
  /* 1 */
  int32_t sub_buffer_pos = 0xFF;
  uint8_t buffer4[] = {2,3,4,6};
  data_length0 = sizeof(buffer4);
  printf("buffer4: ");
  print_buffer(buffer4, sizeof(buffer4));
  ret_val = find_in_rbuff(&rbuff, 0, buffer4, data_length0, &sub_buffer_pos);
  printf("find_in_rbuff(): %d, data_length: %ld, sub_buffer_pos %d\n", ret_val, data_length0, sub_buffer_pos);

  /* flush_rbuff()  */
  printf("\n\n");
  printf("find_in_rbuff() function testing\n");
  /* 1 */
  data_length0 = 11;
  data_length0_before = data_length0;
  ret_val = flush_rbuff(&rbuff, &data_length0);
  printf("flush_rbuff(): %d, data_length0 before: %ld, data_length0 after: %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = 10;
  data_length0_before = data_length0;
  ret_val = flush_rbuff(&rbuff, &data_length0);
  printf("flush_rbuff(): %d, data_length0 before: %ld, data_length0 after: %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = 9;
  data_length0_before = data_length0;
  ret_val = flush_rbuff(&rbuff, &data_length0);
  printf("flush_rbuff(): %d, data_length0 before: %ld, data_length0 after: %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = 8;
  data_length0_before = data_length0;
  ret_val = flush_rbuff(&rbuff, &data_length0);
  printf("flush_rbuff(): %d, data_length0 before: %ld, data_length0 after: %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = 7;
  data_length0_before = data_length0;
  ret_val = flush_rbuff(&rbuff, &data_length0);
  printf("flush_rbuff(): %d, data_length0 before: %ld, data_length0 after: %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = 6;
  data_length0_before = data_length0;
  ret_val = flush_rbuff(&rbuff, &data_length0);
  printf("flush_rbuff(): %d, data_length0 before: %ld, data_length0 after: %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = 5;
  data_length0_before = data_length0;
  ret_val = flush_rbuff(&rbuff, &data_length0);
  printf("flush_rbuff(): %d, data_length0 before: %ld, data_length0 after: %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = 4;
  data_length0_before = data_length0;
  ret_val = flush_rbuff(&rbuff, &data_length0);
  printf("flush_rbuff(): %d, data_length0 before: %ld, data_length0 after: %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);

  data_length0 = 7;
  data_length0_before = data_length0;
  ret_val = flush_rbuff(&rbuff, &data_length0);
  printf("flush_rbuff(): %d, data_length0 before: %ld, data_length0 after: %ld\n", ret_val, data_length0_before, data_length0);
  debug_print_rbuff(&rbuff);
}

void process_data(struct rbuff_t * rbuff) {
  uint8_t cmd_head[] = CMD_HEADER;
  uint8_t cmd_tail[] = CMD_TAIL;
  char cmd_buffer[16];

//  debug_print_rbuff(rbuff);
  while(1) {
    fill_buffer((uint8_t *)cmd_buffer, sizeof(cmd_buffer), 0);
    int32_t cmd_head_pos = 0;
    int32_t cmd_tail_pos = 0;
    int32_t ret_val = 0;

    // 1. Find the head of command
//    printf("\n%s(): stage 1\n", __func__);
    ret_val = find_in_rbuff(rbuff, 0, cmd_head, 2, &cmd_head_pos);
    if (ret_val != 0) {
      printf("find_in_rbuff(): %d\n", ret_val);
      printf("Ring buffer can't be processed\n");
      return;
    }
//    printf("find_in_rbuff(): %d, cmd_head_pos: %d\n", ret_val, cmd_head_pos);

    if (cmd_head_pos == -1) { // There is no command header in buffer, flush it at all (if size of buffer more than header size)
      size_t length = 0;
      length_rbuff(rbuff, &length);
      if (length >= CMD_HEADER_LEN) {
        flush_rbuff(rbuff, &length);
      }
      return;
    }

    // 2. Find the tail of command
//    printf("\n%s(): stage 2\n", __func__);
    ret_val = find_in_rbuff(rbuff, cmd_head_pos + 2, cmd_tail, 1, &cmd_tail_pos);
    if (ret_val != 0) {
      printf("find_in_rbuff(): %d\n", ret_val);
      printf("Ring buffer can't be processed\n");
      return;
    }
//    printf("find_in_rbuff(): %d, cmd_tail_pos: %d\n", ret_val, cmd_tail_pos);

    if (cmd_tail_pos == -1) { // There is no command tail in buffer, wait more
      size_t length = 0;
      length_rbuff(rbuff, &length);
      if (length >= CMD_MAX_LEN) {
        flush_rbuff(rbuff, &length);
      }
      return;
    }

    // 3. Copy data for cmd_head_pos up to cmd_tail_pos including it
//    printf("\n%s(): stage 3\n", __func__);
    size_t cmd_length = 8;
//    uint32_t from = cmd_head_pos;
//    uint32_t to = cmd_tail_pos;
    ret_val = copy_range_from_rbuff(rbuff, (uint8_t *)cmd_buffer, cmd_head_pos, cmd_tail_pos, &cmd_length);
    if (ret_val != 0) {
      printf("copy_range_from_rbuff(): %d\n", ret_val);
      printf("Ring buffer can't be copied with extraction\n");
      return;
    }
//    printf("copy_range_from_rbuff(): %d, from: %d, to: %d, cmd_length: %ld\n", ret_val, from, to, cmd_length);

    // 4. Flush buffer from the head of buffer up to cmd_tail_pos including it
//    printf("\n%s(): stage 4\n", __func__);
    size_t flush_size = cmd_head_pos + cmd_length;
    ret_val = flush_rbuff(rbuff, &flush_size);
    if (ret_val != 0) {
      printf("flush_rbuff(): %d\n", ret_val);
      printf("Ring buffer can't be partially flushed\n");
      return;
    }
//    printf("flush_rbuff(): %d, flush_size: %ld\n", ret_val, flush_size);

    printf("cmd_buffer: %s\n", cmd_buffer);
//    debug_print_rbuff(rbuff);
//    sleep(2);
  }
}

int32_t main() {
//  rbuff_test();
//  return 0;

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

  int32_t option_value = 1;
  // Set socket options
  setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value));

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
  printf("\nListening was started\n");

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
//              printf("Connection request hasn't received\n");
              continue;
          }
          perror("Accepting can't be done");
          close(listen_socket);
          return -4;
      }


    printf("Connection %d was accepted\n", communication_socket);
    struct rbuff_t rbuff;
    uint8_t buffer[16];
    size_t len = sizeof(buffer);
    init_rbuff(&rbuff);
    while (1) {
        if (exit_flag == 1) {
          printf("Program was interrupted\n");
          close(communication_socket);
          close(listen_socket);
  //        exit(-7);
          return -7;
        }

        ssize_t recv_val = recv(communication_socket, buffer, len, MSG_DONTWAIT);
        if (recv_val == -1) { // Receiving finished with error
            if (errno == EAGAIN) { // No data to receive during non blocking receiving
//                printf("Data hasn't received\n");
                sleep(1);
                continue;

            } else { // Receiving finished with error
              perror("Receiving can't be carried out\n");
              close(listen_socket);
              close(communication_socket);
              return -5;
            }

        } else if (recv_val == 0) { // Receiving can't be done due to closed connection
            printf("Connection was closed\n");
            sleep(1);
            break; // Start listening again

        } else { // recv_val > 0 // Receiving finished successfully
          // Data processing
          size_t data_length = recv_val;
          copy_to_rbuff(&rbuff, buffer, &data_length);
          printf("Data received\n");
          process_data(&rbuff);
        }
    } // Receiving loop
    printf("\nListening was started\n");
  } // Accepting loop
//  send();

  close(communication_socket);
  close(listen_socket);

  return 0;
}

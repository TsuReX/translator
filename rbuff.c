#include "rbuff.h"
#include "stdio.h"
/*
struct rbuff_t {
  uint8_t ring_buffer[RBUFF_SIZE];
  uint32_t head;
  uint32_t tail;
  uint32_t full;
};
*/

void init_rbuff(struct rbuff_t * rbuff) {
  rbuff->head = 0;
  rbuff->tail = 0;
  rbuff->full = 0;
}

int32_t copy_to_rbuff(struct rbuff_t * rbuff, const uint8_t * src_buffer, size_t *data_length) {
  if (rbuff == NULL)
    return -1;

  if (src_buffer == NULL)
    return -2;

  if (data_length == NULL)
    return -3;

  if (*data_length == 0)
    return 0;

  if (rbuff->full == 1)
    return -4;

  // head points to byte containing data
  // tail points to free byte
  // tail == head && full == 0 means that buffer is empty
  // tail == head && full == 1 means that buffer is full

  size_t free_length = 0;
  if (rbuff->tail >= rbuff->head) {
    free_length = RBUFF_SIZE - (rbuff->tail - rbuff->head);
  } else { // rbuff->tail < rbuff->head
    free_length = rbuff->head - rbuff->tail;
  }

  if (*data_length > free_length) {
    *data_length = free_length;
  }
  uint32_t i = 0;
  uint32_t tail = rbuff->tail;

  for (; i < *data_length ;) {
    rbuff->ring_buffer[tail &  (RBUFF_SIZE - 1)] = src_buffer[i];
    tail++;
    i++;
  }

  rbuff->tail = tail;

  return 0;
}

int32_t copy_from_rbuff(struct rbuff_t * rbuff, uint8_t * dst_buffer, size_t *data_length, uint32_t remove) {
  if (rbuff == NULL)
    return -1;

  if (dst_buffer == NULL)
    return -2;

  if (data_length == NULL)
    return -3;

  if (*data_length == 0)
    return 0;

  size_t rbuff_length = 0;
  if (rbuff->tail > rbuff->head) {
    rbuff_length = rbuff->tail - rbuff->head;
  } else { // rbuff->tail <= rbuff->head
    rbuff_length = RBUFF_SIZE - (rbuff->head - rbuff->tail);
  }

  if (*data_length > rbuff_length) {
    *data_length = rbuff_length;
  }

  uint32_t i = 0;
  uint32_t head = rbuff->head;

  for (; i < *data_length ;) {
    dst_buffer[i] = rbuff->ring_buffer[head & (RBUFF_SIZE - 1)];
    head++;
    i++;
  }

  if (remove != 0) {
    rbuff->head = head;
  }

  return 0;
}
// int32_t find_in_rbuf(struct rbuff_t *rbuff, const uint8_t * sub_buffer, size_t data_length, uint32_t * sub_buffer_pos);
// int32_t flush_rbuf(struct rbuff_t *rbuff, size_t * flush_length);

void debug_print_rbuff(const struct rbuff_t * rbuff) {
  printf("Ring buffer info:\n");
  printf("\ttotal length: %d\n", RBUFF_SIZE);
  printf("\thead: %d\n", rbuff->head);
  printf("\ttail: %d\n", rbuff->tail);
  printf("\tfull: %d\n", rbuff->full);

  size_t free_length = 0;
  if (rbuff->tail >= rbuff->head) {
    free_length = RBUFF_SIZE - (rbuff->tail - rbuff->head);
  } else { // rbuff->tail < rbuff->head
    free_length = rbuff->head - rbuff->tail;
  }
  printf("\tfree length: %ld\n", free_length);

  size_t rbuff_length = 0;
  if (rbuff->tail > rbuff->head) {
    rbuff_length = rbuff->tail - rbuff->head;
  } else { // rbuff->tail <= rbuff->head
    rbuff_length = RBUFF_SIZE - (rbuff->head - rbuff->tail);
  }
  printf("\tdata length: %ld\n", rbuff_length);

  uint32_t i = 0;
  uint32_t head = rbuff->head;

  printf("Data: ");
  for (; i < rbuff_length ;) {
    printf("0x%X ", rbuff->ring_buffer[head & (RBUFF_SIZE - 1)]);
    head++;
    i++;
  }
  printf("\n");
}

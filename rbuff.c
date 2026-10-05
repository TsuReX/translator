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

// head points to byte containing data
// tail points to free byte
// tail == head && full == 0 means that buffer is empty
// tail == head && full == 1 means that buffer is full
// new elements are added in tail
// stored elements are removed from head

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


  size_t free_length = 0;
  if (rbuff->full == 0) {
    if (rbuff->tail >= rbuff->head) {
      free_length = RBUFF_SIZE - (rbuff->tail - rbuff->head);
    } else { // rbuff->tail < rbuff->head
      free_length = rbuff->head - rbuff->tail;
    }
  } else { // rbuff->full == 1
    free_length = 0;
  }

  if (*data_length > free_length) {
    *data_length = free_length;
    rbuff->full = 1;
  }

  uint32_t tail = rbuff->tail;
  for (uint32_t i = 0; i < *data_length; i++, tail++) {
    rbuff->ring_buffer[tail &  (RBUFF_SIZE - 1)] = src_buffer[i];
  }

  rbuff->tail = tail & (RBUFF_SIZE - 1);

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
  if (rbuff->full == 0) {
    if (rbuff->tail >= rbuff->head) {
      rbuff_length = rbuff->tail - rbuff->head;
    } else { // rbuff->tail < rbuff->head
      rbuff_length = RBUFF_SIZE - (rbuff->head - rbuff->tail);
    }
  } else {
    rbuff_length = RBUFF_SIZE;
  }

  if (*data_length > rbuff_length) {
    *data_length = rbuff_length;
  }

  uint32_t head = rbuff->head;
  for (uint32_t i = 0; i < *data_length; i++, head++) {
    dst_buffer[i] = rbuff->ring_buffer[head & (RBUFF_SIZE - 1)];
  }

  if (remove != 0) {
    rbuff->head = head;
    rbuff->full = 0;
  }

  return 0;
}

int32_t copy_range_from_rbuff(struct rbuff_t * rbuff, uint8_t * dst_buffer, uint32_t head_pos, uint32_t tail_pos, size_t * data_length) {
  if (rbuff == NULL)
    return -1;

  if (dst_buffer == NULL)
    return -2;

  if (data_length == NULL)
    return -3;

  size_t rbuff_length = 0;
  if (rbuff->full == 0) {
    if (rbuff->tail >= rbuff->head) {
      rbuff_length = rbuff->tail - rbuff->head;
    } else { // rbuff->tail < rbuff->head
      rbuff_length = RBUFF_SIZE - (rbuff->head - rbuff->tail);
    }
  } else {
    rbuff_length = RBUFF_SIZE;
  }

  if (*data_length > rbuff_length) {
    *data_length = rbuff_length;
  }

  if (rbuff->full == 0) {

    if (rbuff->tail == rbuff->head) { // [_____T/H_____]
      // Empty
      *data_length = 0;
      return 0;

    } else  if (rbuff->tail > rbuff->head) { // [_____H*****T_____]

      if ((head_pos < rbuff->head) || head_pos > rbuff->tail) { // [__h__H*****T_____] || [_____H*****T__h__]
        return -7;
      }

      if ((tail_pos < rbuff->head) || tail_pos > rbuff->tail) { // [__t__H*****T_____] || [_____H*****T__t__]
        return -8;
      }

    } else { // rbuff->tail < rbuff->head // [*****T_____H*****]

      if (head_pos < rbuff->head) { // [*****T__h__H*****] || [**h**T_____H*****]
        return -9;
      }

      if (tail_pos > rbuff->tail) { // [*****T_____H**t**] || [*****T__t__H*****]
        return -10;
      }
    }

  } else { // rbuff->full == 1 [*****T/H*****]
    if ((head_pos < rbuff->head) || (tail_pos > rbuff->tail)) { // [**h**T/H*****] || [*****T/H**t**]
      return -11;
    }
  }


  uint32_t head = rbuff->head + head_pos;
  uint32_t b_idx = 0;
  for (; (head != tail_pos) && (b_idx < *data_length); b_idx++, head++) {
    dst_buffer[b_idx] = rbuff->ring_buffer[head & (RBUFF_SIZE - 1)];
  }
  if (b_idx < *data_length) {
    dst_buffer[b_idx] = rbuff->ring_buffer[head & (RBUFF_SIZE - 1)];
  }
  *data_length = b_idx;
  return 0;
}

int32_t find_in_rbuff(struct rbuff_t *rbuff, uint32_t rbuff_offset, const uint8_t * sub_buffer, size_t data_length, int32_t * sub_buffer_pos) {
  if (rbuff == NULL)
    return -1;

  if (sub_buffer == NULL)
    return -2;

  if (sub_buffer_pos == NULL)
    return -3;

  if (data_length == 0){
    *sub_buffer_pos = -1;
    return 0;
  }

  size_t rbuff_length = 0;
  if (rbuff->full == 0) {
    if (rbuff->tail >= rbuff->head) {
      rbuff_length = rbuff->tail - rbuff->head;
    } else { // rbuff->tail < rbuff->head
      rbuff_length = RBUFF_SIZE - (rbuff->head - rbuff->tail);
    }
  } else {
    rbuff_length = RBUFF_SIZE;
  }

  if (data_length > rbuff_length)
    return -5;

  if (rbuff_offset > rbuff_length)
    return -6;

  uint32_t match = 0;
  for (uint32_t rb_idx = 0, b_idx = 0, head = rbuff->head + rbuff_offset; rb_idx < rbuff_length ; head++, rb_idx++) {
//    printf("bytes: %d, 0x%02X, 0x%02X\n", head, rbuff->ring_buffer[head & (RBUFF_SIZE - 1)], sub_buffer[b_idx]);
    if (rbuff->ring_buffer[head & (RBUFF_SIZE - 1)] == sub_buffer[b_idx]) {
      match = 1;
      *sub_buffer_pos = head & (RBUFF_SIZE - 1);
      for (head++, b_idx++, rb_idx++; (b_idx < data_length) && (rb_idx < rbuff_length); head++, rb_idx++, b_idx++) {
//        printf("bytes: %d, 0x%02X, 0x%02X\n", _head, rbuff->ring_buffer[head & (RBUFF_SIZE - 1)], sub_buffer[b_idx]);
        if (rbuff->ring_buffer[head & (RBUFF_SIZE - 1)] != sub_buffer[b_idx]) {
          match = 0;
          break;
        }
      } // for (head++, b_idx++, rb_idx++; (b_idx < data_length) && (rb_idx < rbuff_length); head++, rb_idx++, b_idx++)

      if (match == 1) {
        return 0; // for (head++, b_idx++, rb_idx++; (b_idx < data_length) && (rb_idx < rbuff_length); head++, rb_idx++, b_idx++)
      }

    } // (rbuff->ring_buffer[head & (RBUFF_SIZE - 1)] == sub_buffer[b_idx])

  } // for (uint32_t rb_idx = 0, b_idx = 0, head = rbuff->head; rb_idx < rbuff_length ; head++, rb_idx++)
  *sub_buffer_pos = -1;
  return 0;
}

int32_t flush_rbuff(struct rbuff_t *rbuff, size_t * flush_length) {
  if (rbuff == NULL)
    return -1;

  if (flush_length == NULL)
    return -3;

  if (*flush_length == 0)
    return 0;

  size_t rbuff_length = 0;
  if (rbuff->full == 0) {
    if (rbuff->tail >= rbuff->head) {
      rbuff_length = rbuff->tail - rbuff->head;
    } else { // rbuff->tail < rbuff->head
      rbuff_length = RBUFF_SIZE - (rbuff->head - rbuff->tail);
    }
  } else {
    rbuff_length = RBUFF_SIZE;
  }

  if (rbuff_length < *flush_length) {
    *flush_length = rbuff_length;
    rbuff->head = 0;
    rbuff->tail = 0;
  } else {
    rbuff->head = (rbuff->head + *flush_length) & (RBUFF_SIZE - 1);
  }
  rbuff->full = 0;

  return 0;
}

int32_t length_rbuff(struct rbuff_t * rbuff, size_t * length) {
  if (rbuff == NULL)
    return -1;

  if (length == NULL)
    return -3;

  if (rbuff->full == 0) {
    if (rbuff->tail >= rbuff->head) {
      *length = rbuff->tail - rbuff->head;
    } else { // rbuff->tail < rbuff->head
      *length = RBUFF_SIZE - (rbuff->head - rbuff->tail);
    }
  } else {
    *length = RBUFF_SIZE;
  }

  return 0;
}

void debug_print_rbuff(const struct rbuff_t * rbuff) {
  printf("Ring buffer info:\n");
  printf("\ttotal length: %d\n", RBUFF_SIZE);
  printf("\thead: %d\n", rbuff->head);
  printf("\ttail: %d\n", rbuff->tail);
  printf("\tfull: %d\n", rbuff->full);

  size_t free_length = 0;
  size_t rbuff_length = 0;

  if (rbuff->full == 0) {
    if (rbuff->tail >= rbuff->head) {
      free_length = RBUFF_SIZE - (rbuff->tail - rbuff->head);
    } else { // rbuff->tail < rbuff->head
      free_length = rbuff->head - rbuff->tail;
    }

    if (rbuff->tail >= rbuff->head) {
      rbuff_length = rbuff->tail - rbuff->head;
    } else { // rbuff->tail < rbuff->head
      rbuff_length = RBUFF_SIZE - (rbuff->head - rbuff->tail);
    }
  } else {
    free_length = 0;
    rbuff_length = RBUFF_SIZE;
  }

  printf("\tfree length: %ld\n", free_length);
  printf("\tdata length: %ld\n", rbuff_length);


  printf("Data from head to tail: ");
  for (uint32_t i = 0, head = rbuff->head; i < rbuff_length; i++, head++) {
    printf("0x%02X ", rbuff->ring_buffer[head & (RBUFF_SIZE - 1)]);
  }
  printf("\n");

  printf("Data from 0 to RBUFF_SIZE: ");
  for (uint32_t i = 0; i < RBUFF_SIZE; i++) {
    printf("0x%02X ", rbuff->ring_buffer[i]);
  }
  printf("\n");
}

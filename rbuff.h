#ifndef _RBUFF_H_
#define _RBUFF_H_

#include <stdint.h>
#include <stddef.h>

#define RBUFF_SIZE 0x40

struct rbuff_t {
  uint8_t ring_buffer[RBUFF_SIZE];
  uint32_t head;
  uint32_t tail;
  uint32_t full;
//  TODO: Implement using the following variable
//  it will allow to avoid using "full" variable
//  (rbuff_length == RBUFF_SIZE -> buffer is full)
//  and 'rbuff_length" variable evaluation each time
//  size_t rbuff_length;
};

void init_rbuff(struct rbuff_t * rbuff);
int32_t copy_to_rbuff(struct rbuff_t * rbuff, const uint8_t * src_buffer, size_t *data_length);
int32_t copy_from_rbuff(struct rbuff_t * rbuff, uint8_t * dst_buffer, size_t *data_length, uint32_t remove);
int32_t find_in_rbuff(struct rbuff_t *rbuff, const uint8_t * sub_buffer, size_t data_length, int32_t * sub_buffer_pos);
int32_t flush_rbuff(struct rbuff_t *rbuff, size_t * flush_length);
void debug_print_rbuff(const struct rbuff_t * rbuff);
#endif // _RBUFF_H_

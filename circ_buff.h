#ifndef _CIRC_BUFF_
#define _CIRC_BUFF_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    uint8_t     data_size;
    uint32_t    data_cur_length;
    uint32_t    data_max_length;
    uint8_t *   begin;
    uint8_t *   end;
    uint8_t *   read_head;
    uint8_t *   write_head;
} circ_buff;

uint8_t circ_buff_init(circ_buff *, uint8_t, uint32_t, void *);
uint8_t circ_buff_write(circ_buff *, uint32_t, void *);
uint8_t circ_buff_read(circ_buff *, uint32_t, void *, bool);

#endif
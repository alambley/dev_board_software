#include "circ_buff.h"

uint8_t circ_buff_init(circ_buff * p_to_init, uint8_t data_size, uint32_t data_length, void * p_memory) {
    p_to_init->data_size = data_size;
    p_to_init->data_cur_length = 0;
    p_to_init->data_max_length = data_length;
    p_to_init->begin = (uint8_t *) p_memory;
    p_to_init->write_head = p_to_init->begin;
    p_to_init->read_head = p_to_init->begin;
    p_to_init->end = p_to_init->begin + (p_to_init->data_size * p_to_init->data_max_length);
    return 0;
}

uint8_t circ_buff_write(circ_buff * p_to_init, uint32_t length, void * p_data) {
    // If length of data to write wouldn't fit in buffer at all, error out
    if(length > p_to_init->data_max_length){
        return 1;
    }
    // If length of data would would need to wrap back to start
    if(p_to_init->write_head + length * p_to_init->data_size > p_to_init->end){
        // Get size of both copies
        uint32_t first_copy_size = p_to_init->end - p_to_init->write_head;
        uint32_t second_copy_size = length - first_copy_size;
        // Do first copy from write_head to end
        memcpy(p_to_init->write_head, (char *)p_data,  first_copy_size * p_to_init->data_size);
        // Do second copy from begin to end of length
        memcpy(p_to_init->begin, (char *)p_data + first_copy_size * p_to_init->data_size, second_copy_size * p_to_init->data_size);

        // Increment write_head
        p_to_init->write_head = p_to_init->begin + second_copy_size * p_to_init->data_size;
    }else{
        // Copy data
        memcpy(p_to_init->write_head, (char *)p_data,  p_to_init->data_size * length);
        // Increment write head
        p_to_init->write_head += p_to_init->data_size * length;
        
    }

    // Increment cur_length
    p_to_init->data_cur_length += length;

    // If write overwrote data, read_head == write_head and cur_length = max_length    
    if(p_to_init->data_cur_length > p_to_init->data_max_length){
        p_to_init->read_head = p_to_init->write_head;
        p_to_init->data_cur_length = p_to_init->data_max_length;
    }
    return 0;
}

uint8_t circ_buff_read(circ_buff * p_to_init, uint32_t length, void * p_data, bool increment_read_head) {
    // If length is empty, return error
    if(p_to_init->data_cur_length == 0) {
        return 1;
    }
    // If length is larger than buffer, return error
    if(p_to_init->data_max_length < length) {
        return 2;
    }
    
    // If length of data would would need to wrap back to start
    if(p_to_init->read_head + length * p_to_init->data_size > p_to_init->end){
        // Get size of both copies
        uint32_t first_copy_size = p_to_init->end - p_to_init->read_head;
        uint32_t second_copy_size = length - first_copy_size;
        // Do first copy from write_head to end
        memcpy((char *)p_data, p_to_init->write_head, first_copy_size * p_to_init->data_size);
        // Do second copy from begin to end of length
        memcpy((char *)p_data + first_copy_size * p_to_init->data_size, p_to_init->begin, second_copy_size * p_to_init->data_size);
        // If user wants to increment read_head
        if(increment_read_head){
            // Increment read head
            p_to_init->read_head = p_to_init->begin + second_copy_size * p_to_init->data_size;
            // Decrement cur_length
            p_to_init->data_cur_length -= length;
        }
    }else{
        // Copy data
        memcpy((char *)p_data, p_to_init->read_head, p_to_init->data_size * length);
        // If user wants to increment read_head
        if(increment_read_head){
            // Increment read head
            p_to_init->read_head += p_to_init->data_size * length;
            // Decrement cur_length
            p_to_init->data_cur_length -= length;
        }
    }
    return 0;
}

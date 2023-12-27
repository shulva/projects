#ifndef HEADER_RING_BUFFER_H
#define HEADER_RING_BUFFER_H

#include "types.h"
#include "sync.h"
#include "thread.h"

#define buffer_size 64

typedef struct ring_buffer
{
    lock lock;
    thread_pcb *producer;
    thread_pcb *consumer;

    char buffer[buffer_size];
    int32_t head_pointer;
    int32_t tail_pointer;
}ring_buffer;

void ring_buffer_init(ring_buffer *buffer);
// return the next position in buffer
int32_t next_position(int32_t pos);

_Bool ring_buffer_is_full(ring_buffer * buffer);

_Bool ring_buffer_is_empty(ring_buffer * buffer);

void ring_buffer_wait(thread_pcb** waiter);

void wake_up(thread_pcb ** waiter);
//consumer get char from buffer
char getchar_from_buffer(ring_buffer * buffer);
//producer put char into buffer
void putchar_into_buffer(ring_buffer * buffer,char key);

#endif
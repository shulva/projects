#include "../header/ring_buffer.h"
#include "../header/idt.h"
#include "../header/console.h"

void ring_buffer_init(ring_buffer *buffer)
{
    lock_init(&buffer->lock);
    buffer->producer=buffer->consumer=NULL;
    buffer->head_pointer=buffer->tail_pointer=0;
}

int32_t next_position(int32_t pos)
{
    return (pos+1)%buffer_size;
}

_Bool ring_buffer_is_full(ring_buffer * buffer)
{
    assert(interrupt_get_status()==INTR_OFF,"interrupt is on");
    return next_position(buffer->head_pointer)==buffer->tail_pointer;
}

_Bool ring_buffer_is_empty(ring_buffer * buffer)
{
    assert(interrupt_get_status()==INTR_OFF,"interrupt is on");
    return buffer->head_pointer==buffer->tail_pointer;
}

void ring_buffer_wait(thread_pcb** waiter)
{
    assert(*waiter==NULL && waiter !=NULL,"wait-waiter");
    *waiter=get_running_thread_pcb();
    thread_blocked(TASK_BLOCKED);
}

void wake_up(thread_pcb ** waiter)
{
    assert(*waiter!=NULL ,"wait-waiter");
    thread_unblocked(*waiter);
    waiter=NULL;
}
//consumer get char from buffer
char getchar_from_buffer(ring_buffer * buffer)
{
    assert(interrupt_get_status()==INTR_OFF,"interrupt is on");

    while (ring_buffer_is_empty(buffer))
    {
        lock_acquire(&buffer->lock);
        ring_buffer_wait(&buffer->consumer);
        lock_release(&buffer->lock);
    }

    char key=buffer->buffer[buffer->tail_pointer];
    buffer->tail_pointer=next_position(buffer->tail_pointer);

    if(buffer->producer!=NULL)
    {
        wake_up(&buffer->producer);
    }

    return key;

    
}
//producer put char into buffer
void putchar_into_buffer(ring_buffer * buffer,char key)
{
    assert(interrupt_get_status()==INTR_OFF,"interrupt is on");

    while (ring_buffer_is_full(buffer))
    {
        lock_acquire(&buffer->lock);
        ring_buffer_wait(&buffer->producer);
        lock_release(&buffer->lock);
    }

    buffer->buffer[buffer->tail_pointer]=key;
    buffer->head_pointer=next_position(buffer->head_pointer);

    if(buffer->consumer!=NULL)
    {
        wake_up(&buffer->consumer);
    }
}
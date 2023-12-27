#ifndef HEADER_SYNC_H
#define HEADER_SYNC_H

#include "stl.h"
#include "types.h"
#include "thread.h"
#include "idt.h"
#include "console.h"

typedef struct semaphore
{
    uint8_t value;
    double_linked_list waiter;
} semaphore;

typedef struct lock
{
    struct thread_pcb *lock_holder;
    semaphore semaphore;
    uint32_t repeat_num_lock_holder;
} lock;

void semaphore_init(semaphore *semaphore, uint8_t value);

void lock_init(lock *lock);

void semaphore_sub(semaphore *p_semaphore);

void semaphore_add(semaphore *p_semaphore);

void lock_acquire(lock *lock);

void lock_release(lock *lock);

#endif
#include "../header/sync.h"

void semaphore_init(semaphore *semaphore, uint8_t value)
{
    semaphore->value = value;

    list_init(&semaphore->waiter);
}

void lock_init(lock *lock)
{
    lock->lock_holder = NULL;
    lock->repeat_num_lock_holder = 0;
    semaphore_init(&lock->semaphore, 1);
}

void semaphore_sub(semaphore *p_semaphore)
{
    // atomic
    interrupt_status old_status = interrupt_disable();

    while (p_semaphore->value == 0)
    {
        assert(!node_find(&p_semaphore->waiter, &get_running_thread_pcb()->tag), "\nblocked thread do  exist in waiter");

        if (node_find(&p_semaphore->waiter, &get_running_thread_pcb()->tag))
        {
            panic("blocked thread do exist in waiter", "sync.c", 28);
        }

        list_append(&p_semaphore->waiter, &get_running_thread_pcb()->tag);
        thread_blocked(TASK_BLOCKED); // blocked the thread
    }

    p_semaphore->value--;

    interrupt_set_status(old_status);
}

void semaphore_add(semaphore *p_semaphore)
{
    // atomic
    interrupt_status old_status = interrupt_disable();

    assert(p_semaphore->value == 0, "value is not 0");

    if (!list_empty(&p_semaphore->waiter))
    {
        thread_pcb *thread_blocked = element_entry(thread_pcb, tag, list_pop(&p_semaphore->waiter));
        thread_unblocked(thread_blocked);
    }

    p_semaphore->value++;

    interrupt_set_status(old_status);
}

void lock_acquire(lock *lock)
{

    if (lock->lock_holder != get_running_thread_pcb())
    {
        semaphore_sub(&lock->semaphore);
        lock->lock_holder = get_running_thread_pcb();

        assert(lock->repeat_num_lock_holder == 0, "lock_holder exist");
        lock->repeat_num_lock_holder = 1;
    }
    else
    {
        lock->repeat_num_lock_holder++;
    }

    // console_write_hex(lock->lock_holder,black,white);
    // console_write("\n");
    // console_write_hex(get_running_thread_pcb(),black,white);
    // console_write("\n");
}

void lock_release(lock *lock)
{
    assert(lock->lock_holder == get_running_thread_pcb(), "running thread is not lock holder");

    // console_write_hex(lock->lock_holder,black,white);
    // console_write("\n");
    // console_write_hex(get_running_thread_pcb(),black,white);
    // console_write("\n");

    if (lock->repeat_num_lock_holder > 1)
    {
        lock->repeat_num_lock_holder--;
        return;
    }

    assert(lock->repeat_num_lock_holder == 1, "repeat_num_lock_holder >1");

    lock->lock_holder = NULL;
    lock->repeat_num_lock_holder = 0;
    semaphore_add(&lock->semaphore);
}
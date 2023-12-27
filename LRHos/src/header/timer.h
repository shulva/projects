#ifndef HEADER_TIMER_H
#define HEADER_TIMER_H

#include "../header/types.h"

void init_timer(uint32_t frequency);
void ms_sleep(uint32_t ms);
static void ticks_to_sleep(uint32_t sleep_ticks);
void loop_sleep(uint8_t times);
#endif
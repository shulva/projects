#ifndef HEADER_KEYBOARD_H
#define HEADER_KEYBOARD_H

#include "ring_buffer.h"
#include "idt.h"

void keyboard_init();

extern ring_buffer keyboard_buffer;

#endif
#ifndef HEADER_PORT_H
#define HEADER_PORT_H

#include "types.h"

void outbyte(uint16_t port, uint8_t value); //端口写一字节
uint8_t inbyte(uint16_t port);              //端口读一字节
uint16_t inword(uint16_t port);             //端口读一个字
void outsw(uint16_t port, const void *addr, uint32_t num);
void insw(uint16_t port, void *addr, uint32_t num);

#endif

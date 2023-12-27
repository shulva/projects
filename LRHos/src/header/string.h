#ifndef HEADER_STRING_H_
#define HEADER_STRING_H_

#include "types.h"
#include "console.h"
//具体设计请参考https://en.cppreference.com/
///////////////////////////

// dest	-	pointer to the character array to write to
// returns a copy of dest
char *strcpy(char *dest, const char *src);

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);

void memset(void *dest, uint8_t val, uint32_t len);

int strcmp(const char *str1, const char *str2);

char *strcat(char *dest, const char *src);

int strlen(const char *src);

/* 将dst中len个相邻字节位置交换后存入buf */
void swap_bytes(const char *dst, char *buffer, uint32_t length);

#endif
#include "../header/string.h"

inline void memset(void *dest, uint8_t val, uint32_t len)
{
    // assert(dest != NULL, "dest=null");
    uint8_t *dst = (uint8_t *)dest;

    for (; len != 0; len--)
    {
        *dst++ = val;
    }
}

inline void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
    for (; len != 0; len--)
    {
        *dest++ = *src++;
    }
}

inline int strcmp(const char *str1, const char *str2)
{
    int ret = 0;

    while (!(ret = *str1 - *str2) && *str1)
    {
        str1++;
        str2++;
    }

    if (ret < 0)
    {
        return -1; // str2 is bigger
    }
    else if (ret > 0)
    {
        return 1; // str1 bigger
    }

    return 0; // equal
}

inline char *strcpy(char *dest, const char *src)
{
    char *pointer = NULL;

    if (dest == NULL && src == NULL)
    {
        return NULL;
    }

    pointer = dest;

    while ((*dest++ = *src++) == '\0')
        ;

    return pointer;
}

inline int strlen(const char *src)
{
    int i = 0;

    if (src == NULL)
        return NULL;

    while (*src++ != '\0')
    {
        i++;
    }

    return i;
}

void swap_bytes(const char *dst, char *buffer, uint32_t length)
{
    uint8_t index;
    for (index = 0; index < length; index += 2)
    {
        // buffer 中存储 dst 中两相邻元素交换位置后的字符串
        buffer[index + 1] = *dst++;
        buffer[index] = *dst++;
    }

    buffer[index] = '\0';
}
#include <stdio.h>
#include <string.h>

int my_memmove(void *dst, const void *src, size_t size)
{
    char buffer[size];
    char *c_src = (char *)src;
    char *c_dst = (char *)dst;

    for (size_t i = 0; i < size; i++)
    {
        buffer[i] = c_src[i];
    }

    for (size_t i = 0; i < size; i++)
    {
        *(c_dst + i) = buffer[i];
    }

    return 0;
}

int main(void)
{
    char text[] = "bacana";
    my_memmove(&text[2], text, 4);

    printf("result: %s\n", text);
}


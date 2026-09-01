#include <stddef.h>
#include <stdio.h>

void *my_memcpy(size_t n, void *dest, void *src)
{
    char *s = (char *)src;
    char *d = (char *)dest;

    for (size_t i = 0; i < n; i++)
    {
        *(d + i) = *(s + i);
    }

    return dest;
}

int main(void)
{
    char *t = "test\n";
    char c[6];
    char *copy = my_memcpy(6, (void*)c, (void *)t);
    printf("copied: %s\n", copy);
}

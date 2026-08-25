#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong arg count. Usage: ./strlen \"text to get len\"");
    }

    int str_len = 0;
    char *text = argv[1];
    char c;
    while (1)
    {
        c = text[str_len];
        if (c == '\0')
        {
            break;
        }

        str_len++;
    }

    printf("your string has len %d\n", str_len);
}

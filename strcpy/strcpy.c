#include <stddef.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

int my_strcpy(char *orig, char *copy, size_t size)
{
    if (size == 0)
        return FALSE;

    for (char *c = orig; *c != '\0'; c++)
    {
        size_t index = (size_t)(c - orig);
        if (index >= size - 1)
        {
            return FALSE;
        }
        copy[index] = *c;
    }
    copy[size -1] = '\0';

    return TRUE;
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong arg count. Usage: ./strcpy text\n");
        return 1;
    }

    size_t size = 0;
    for (char *c = argv[1]; *c != '\0'; c++)
    {
        size++;
    }
    size++; // for '\0'

    char copy[size];
    int ok = my_strcpy(argv[1], copy, size);
    if (!ok)
    {
        printf("failed copying %s\n", argv[1]);
        return 1;
    }

    printf("the copied str is: %s\n", copy);
}

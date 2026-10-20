#include "dynamic-array.h"
#include <stdio.h>

int main(void)
{
    struct dyn_arr arr = init_arr(3, sizeof(int));

    push_arr(&arr, &(int){3});
    push_arr(&arr, &(int){2});
    push_arr(&arr, &(int){1});
    push_arr(&arr, &(int){0});

    int poped0 = *(int*)pop_arr(&arr);
    int poped1 = *(int*)pop_arr(&arr);
    int poped2 = *(int*)pop_arr(&arr);
    int poped3 = *(int*)pop_arr(&arr);

    printf("popped0: %d, popped1: %d, popped2: %d, poped3: %d\n", poped0, poped1, poped2, poped3);

    free_arr(&arr);

    struct dyn_arr arr2 = init_arr(1, sizeof(int));

    push_arr(&arr2, &(int){2});
    push_arr(&arr2, &(int){1});
    push_arr(&arr2, &(int){0});

    printf("arr2 expanded to cap %zu \n", arr2.capacity);
    free_arr(&arr2);
}

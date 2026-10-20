#include "dynamic-array.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// struct dyn_arr {
//     int *pointer;
//     size_t length;
//     size_t capacity;
//     size_t type_size;
// };

struct dyn_arr init_arr(size_t capacity, size_t type_size)
{
	void *alloc = calloc(capacity, type_size);
	struct dyn_arr arr = {.pointer = alloc,
						  .length = 0,
						  .capacity = capacity,
						  .type_size = type_size};

	return arr;
}

void free_arr(struct dyn_arr *arr) { free(arr->pointer); }

int push_arr(struct dyn_arr *arr, void *item)
{
    char *for_arit = (char *)arr->pointer;
	if (arr->length >= arr->capacity)
	{
		size_t new_cap = arr->capacity * 2;
		void *new_buf = realloc(arr->pointer, new_cap);
		if (new_buf == NULL)
		{
			perror("realloc");
			return -1;
		}
		memset(for_arit + arr->capacity * arr->type_size, 0,
			   arr->capacity * arr->type_size);

        arr->capacity = new_cap;
	}

    memcpy(for_arit + arr->length * arr->type_size, item, arr->type_size);
    arr->length++;

    return 0;
}

void *pop_arr(struct dyn_arr *arr) 
{
    char *for_arit = (char *)arr->pointer;
    void *popped = for_arit + (arr->length -1) * arr->type_size;
    arr->length--;
    return popped;
}

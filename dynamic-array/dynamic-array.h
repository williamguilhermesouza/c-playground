#ifndef DAR_H

#define DAR_H
#include <sys/types.h>

struct dyn_arr {
    void *pointer;
    size_t length;
    size_t capacity;
    size_t type_size;
};

struct dyn_arr init_arr(size_t capacity, size_t type_size);
void free_arr(struct dyn_arr *arr);
int push_arr(struct dyn_arr *arr, void *item);
void *pop_arr(struct dyn_arr *arr);

#endif

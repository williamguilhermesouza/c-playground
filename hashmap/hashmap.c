#include <stdio.h>

#define HASH_FACTOR 5381

struct Entry {
    char *key; // hashed key
    int value; 
};

struct HashMap {
    struct Entry map[10];
};

static int djb2_hash(char *key, size_t map_size)
{
    // beware of overflow
    long current_hash = HASH_FACTOR;

    for (char *p = key; *p != '\0'; p++)
    {
        current_hash = (current_hash * 33) + *p;
    }

    return (int) current_hash % map_size;
}


int main(void)
{
    struct Entry e1 = { .key="first", .value=1 };
    //struct Entry e2 = { .key="second", .value=2 }; // collision
    struct Entry e2 = { .key="third", .value=2 }; 

    int hash1 = djb2_hash(e1.key, 10);
    int hash2 = djb2_hash(e2.key, 10);

    printf("first hash %d\n", hash1);
    printf("second hash %d\n", hash2);

    struct HashMap map;

    map.map[hash1] = e1;
    map.map[hash2] = e2;

    // to find item with second key 
    int hash_find = djb2_hash("third", 10);

    struct Entry item_found = map.map[hash_find];
    printf("found entry with value %d\n", item_found.value);
}

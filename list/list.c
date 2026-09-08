#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list
{
	void *data;
	int capacity;
	int size;
	size_t el_size;
};

struct list list_create(int capacity, size_t el_size)
{
	struct list l;
	l.capacity = capacity;
	l.el_size = el_size;
	l.size = 0;

	l.data = calloc(el_size, l.capacity);
	return l;
}

int list_add(struct list *l, void *el)
{
	if (l->size == l->capacity)
	{
		int new_cap = (l->capacity == 0) ? 4 : l->capacity * 2;

		void *p = realloc(l->data, new_cap * l->el_size);
		if (p == NULL)
		{
			perror("realloc");
			return -1;
		}

		l->data = p;
		l->capacity = new_cap;
	}

	unsigned char *p = (unsigned char *)l->data + (l->size * l->el_size);

	memcpy(p, el, l->el_size);
	l->size++;

	return 0;
}

void *list_get(const struct list *l, int index)
{
	if (index < 0 || index >= l->size)
	{
		fprintf(stderr, "out of list bounds");
		return NULL;
	}

	void *p = (char *)l->data + (l->el_size * index);
	return p;
}

int list_remove(struct list *l, int index)
{
	if (index < 0 || index >= l->size)
	{
		fprintf(stderr, "out of list bounds");
		return -1;
	}

	if (index != l->size - 1)
	{
		int diff = l->size - index - 1;
		void *p = (char *)l->data + (index * l->el_size);
		void *pnext = (char *)p + (1 * l->el_size);
        memmove(p, pnext, diff * l->el_size);
	}

	void *plast = (char *)l->data + ((l->size - 1) * l->el_size);
	memset(plast, 0, l->el_size);

	l->size--;
	return 0;
}

void list_free(struct list *l) { free(l->data); }

int main(void)
{
	struct list li = list_create(3, sizeof(int));
	struct list *l = &li;

	for (int i = 0; i < 4; i++)
	{
		list_add(l, &i);
	}

	int *a = (int *)list_get(l, 3);
	int *b = (int *)list_get(l, 2);
	int *c = (int *)list_get(l, 1);
	int *d = (int *)list_get(l, 0);

	printf("a: %d, b: %d, c: %d, d: %d\n", *a, *b, *c, *d);

	list_remove(l, 2);
	b = (int *)list_get(l, 2);
	c = (int *)list_get(l, 1);
	d = (int *)list_get(l, 0);

	printf("b: %d, c: %d, d: %d\n", *b, *c, *d);
	list_free(l);
}

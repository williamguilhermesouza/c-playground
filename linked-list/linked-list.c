#include <stdio.h>

struct node
{
	void *data;
	struct node *next;
};

void ll_add(struct node *head, struct node *el)
{
	struct node *last, *p;
	for (p = head; p != NULL; p = p->next)
	{
		last = p;
	}

	last->next = el;
}

void *ll_get(struct node *head, int index)
{
	struct node *p;

	if (index < 0)
	{
		return NULL;
	}

	for (p = head; p != NULL; p = p->next)
	{
		if (index == 0)
		{
			return p->data;
		}

		index--;
	}

	return NULL;
}

void *ll_pop(struct node **head, int index)
{
	struct node *p, *prev;

	if (index < 0)
	{
		return NULL;
	}

	prev = NULL;
	for (p = *head; p != NULL; p = p->next)
	{
		if (index == 0)
		{
			void *data = p->data;
			if (prev == NULL)
			{
				*head = p->next;
			}
			else
			{
				prev->next = p->next;
			}

			return data;
		}

		prev = p;
		index--;
	}

	return NULL;
}

void print_list(struct node *head)
{
	struct node *p;
	int i = 0;

	for (p = head; p != NULL; p = p->next)
	{
		printf("%d: %d\t", i, *(int *)p->data);
		i++;
	}
	printf("\n");
}

int main(void)
{

	struct node n2 = {.data = &(int){5}, .next = NULL};
	struct node n1 = {.data = &(int){4}, .next = &n2};
	struct node head = {.data = &(int){3}, .next = &n1};
	print_list(&head);

	struct node n3 = {.data = &(int){6}, .next = NULL};
	ll_add(&head, &n3);
	print_list(&head);

	void *item = ll_get(&head, 1);
	printf("got item %d on index 1\n", *(int *)item);

	struct node *p = &head;
	ll_pop(&p, 1);
	print_list(&head);
}

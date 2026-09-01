#include <stdio.h>

int my_strcmp(char *s1, char *s2)
{
	while (*s1 == *s2)
	{
		if (*s1 == '\0')
			return 0;

		s1++;
		s2++;
	}

	return *s1 - *s2;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("wrong arg count. usage: ./strcmp string1 string2\n");
		return 1;
	}

	int res = my_strcmp(argv[1], argv[2]);
    printf("the compare res is: %d\n", res);
}

CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic
DEPS := dynamic-array.h
OBJ := dynamic-array.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f main
	rm -f $(OBJ)




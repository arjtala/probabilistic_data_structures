CC = gcc
CFLAGS = -Wall -Wextra -Werror

.o: @.c @.h
	$(CC) $(CFLAGS) $@.c -o $@

OBJECTS = bitvector.o bloom.o hash.o

test: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) tests.c -o tests.out

clean:
	@rm -f *.o

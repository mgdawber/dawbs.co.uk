CC=gcc
CFLAGS=-I.
DEPS = src/main.h
OBJ = src/main.o src/file_utils.o src/dir_utils.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

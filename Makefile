CC = gcc

CFLAGS += -Wall -fno-stack-protector -Wextra -g

.PHONY: all clean

# Build
all: release

release: mainRelease

mainRelease:
	$(CC) $(CFLAGS) Main.c -o mainlab5 $(LIBS)

clean:
	-rm -f mainlab5

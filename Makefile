CC := gcc
CFLAGS := -g -Wall -luring

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

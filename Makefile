CC := gcc
CFLAGS := -O2 -Wall -luring

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

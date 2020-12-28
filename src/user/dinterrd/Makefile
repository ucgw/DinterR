CC=gcc
CFLAGS=-std=gnu99 -Wall -g -I../include

TARGET=dinterrd
DINTERR_FILE=bigfile

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

.c.o:
	$(CC) -o $@ -c $*.c

clean:
	rm -f $(TARGET) $(DINTERR_FILE)

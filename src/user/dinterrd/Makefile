CC=gcc
CFLAGS=-std=gnu99 -Wall -g -I./common/include

TARGET=inotify_test
DINTERR_FILE=bigfile

all: $(DINTERR_FILE) $(TARGET)

test: $(DINTERR_FILE) $(TARGET)
	./$(TARGET) $(DINTERR_FILE)

$(DINTERR_FILE):
	./gen_gbfile.sh

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

.c.o:
	$(CC) -o $@ -c $*.c

clean:
	rm -f $(TARGET) $(DINTERR_FILE)

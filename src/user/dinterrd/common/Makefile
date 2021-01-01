CC=g++
#CFLAGS=-I./include -std=c++2a -Wall -pedantic -g -lz
CFLAGS=-I./include -std=c++2a -pedantic -g -lz
OBJS=dataproto.o dataproto_main.o

TARGET1=dataproto_test

$(TARGET1): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) $(TARGET1)

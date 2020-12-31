CC=g++
#CFLAGS=-I./include -std=c++2a -Wall -pedantic -g -lz
CFLAGS=-I./include -std=c++2a -pedantic -g -lz
OBJS=proto.o main.o

TARGET=proto_test

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) $(TARGET)

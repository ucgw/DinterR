CC=g++
#CFLAGS=-I./include -std=c++2a -Wall -pedantic -g -lz
CFLAGS=-I./include -std=c++2a -pedantic -g -lz
OBJS=dataproto.o main.o

TARGET1=data_proto_test

$(TARGET1): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) $(TARGET1)

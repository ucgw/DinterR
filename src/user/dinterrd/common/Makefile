CC=g++
#CFLAGS=-I./include -std=c++2a -Wall -pedantic -g -lz
CFLAGS=-I./include -std=c++2a -pedantic -g -lz
OBJS=serdes.o dataproto.o dataproto_main.o

TARGET1=dataproto_test

$(TARGET1): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS)

objs: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) $(TARGET1)

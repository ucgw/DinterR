CC=g++
#CFLAGS=-I./include -std=c++2a -Wall -pedantic -g -lz
CFLAGS=-I./include -std=c++2a -pedantic -g -lz
OBJS_DATAPROTO=serdes.o dataproto.o dataproto_main.o
OBJS_NETPROTO=serdes.o sockio.o netproto_main.o sig.o server.o thread.o

TARGET1=dataproto_test
TARGET2=netproto_test


all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS_DATAPROTO)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS_DATAPROTO)

$(TARGET2): $(OBJS_NETPROTO)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJS_NETPROTO)

objs: $(OBJS_DATAPROTO) $(OBJS_NETPROTO)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS_DATAPROTO) $(OBJS_NETPROTO) $(TARGET1) $(TARGET2)

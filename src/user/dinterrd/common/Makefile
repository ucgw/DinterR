CC=g++
#CFLAGS=-I./include -std=c++2a -Wall -pedantic -g -lz
CFLAGS=-I./include -std=c++2a -pedantic -g -lz -lpthread
OBJS_DATAPROTO=serdes.o dataproto.o dataproto_test.o
OBJS_NETPROTO=serdes.o sockio.o netproto_test.o sig.o server.o thread.o lock.o
OBJS_CLIENT=sockio.o serdes.o server.o thread.o client_test.o payload.o lock.o

TARGET1=dataproto_test
TARGET2=netproto_test
TARGET3=client_test


all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(OBJS_DATAPROTO)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS_DATAPROTO)

$(TARGET2): $(OBJS_NETPROTO)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJS_NETPROTO)

$(TARGET3): $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o $(TARGET3) $(OBJS_CLIENT)

objs: $(OBJS_DATAPROTO) $(OBJS_NETPROTO)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS_CLIENT) $(OBJS_DATAPROTO) $(OBJS_NETPROTO) $(TARGET1) $(TARGET2) $(TARGET3)

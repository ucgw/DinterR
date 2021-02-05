CC=g++
#CFLAGS=-I./include -std=c++2a -Wall -pedantic -g -lz
CFLAGS=-I../common/include -L../common -std=c++2a -pedantic -g -lz -lpthread
OBJS_SERVER=../common/serdes.o ../common/sockio.o ../common/server.o ../common/thread.o ../common/lock.o ../common/payload.o ddtp_serve.o

TARGET1=ddtp-serve

all: $(TARGET1)

$(TARGET1): $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS_SERVER)

objs: $(OBJS_CLIENT)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(TARGET1) *.o

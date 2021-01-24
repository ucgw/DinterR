CC=g++
#CFLAGS=-I./include -std=c++2a -Wall -pedantic -g -lz
CFLAGS=-I../common/include -L../common -std=c++2a -pedantic -g -lz -lpthread
OBJS_CLIENT=../common/client.o ../common/sockio.o ../common/serdes.o ../common/payload.o ../common/lock.o ddtp_cli.o

TARGET1=ddtp-cli

all: $(TARGET1)

$(TARGET1): $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS_CLIENT)

objs: $(OBJS_CLIENT)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(TARGET1) *.o

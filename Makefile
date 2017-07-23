all:echo_server

CC=g++
CPPFLAGS=-Wall -std=c++11 -ggdb
LDFLAGS=-pthread

echo_server:server.o
	$(CC) $(LDFLAGS) -o $@ $^

server.o:server.cpp
	$(CC) $(CPPFLAGS) -o $@ -c $^

.PHONY:
	clean

clean:
	rm sever.o echo_server

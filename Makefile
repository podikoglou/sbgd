CC = g++
CFLAGS = -O2 -Wall -std=c++17

sbgd: sbgd.cpp
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm sbgd

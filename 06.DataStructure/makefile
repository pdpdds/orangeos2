all: List.o Queue.o Stack.o

CC		= i686-elf-gcc
CPP		= i686-elf-g++

CFLAGS		= -fno-pic -m32 -Wall -Winline -O3 -fno-keep-inline-functions -fomit-frame-pointer -fstrength-reduce \
		-ffreestanding -fno-exceptions -nostdinc -D $(ARCH)
CPPFLAGS	= -I../include -Wno-comment -Wno-write-strings -Wno-narrowing -Wall -Wpointer-arith -Wcast-align -Wno-unused-parameter -nostdlib -fno-builtin -fno-rtti -fno-exceptions -Wno-unused-value -Wno-unused-but-set-variable -Wno-address-of-packed-member

List.o: List.c
	$(CPP) $(CPPFLAGS) -c List.c -o List.o	

Queue.o: Queue.cpp
	$(CPP) $(CPPFLAGS) -c Queue.cpp -o Queue.o	

Stack.o: Stack.cpp
	$(CPP) $(CPPFLAGS) -c Stack.cpp -o Stack.o	

clean:
	rm -f List.o
	rm -f Queue.o
	rm -f Stack.o
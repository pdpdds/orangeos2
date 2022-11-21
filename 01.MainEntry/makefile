all: MainEntry.o

MainEntry.o: MainEntry.cpp
	i686-elf-g++ -I../include -fno-use-cxa-atexit -Wno-comment -Wno-write-strings -W -Werror -Wall -Wpointer-arith -Wcast-align -Wno-unused-parameter -nostdlib -fno-builtin -fno-rtti -fno-exceptions -c MainEntry.cpp -o MainEntry.o

clean:
	rm -f MainEntry.o
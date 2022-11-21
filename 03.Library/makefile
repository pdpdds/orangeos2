all: IO.o Common.o Console.o Util.o IStream.o OStream.o printf.o Util.o

CC		= i686-elf-gcc
CPP		= i686-elf-g++

CFLAGS		= -fno-pic -m32 -Wall -Winline -O3 -fno-keep-inline-functions -fomit-frame-pointer -fstrength-reduce \
		-ffreestanding -fno-exceptions -nostdinc -D $(ARCH)
CPPFLAGS	= -I../include -Wno-comment -Wno-write-strings -Wno-narrowing -Wall -Wpointer-arith -Wcast-align -Wno-unused-parameter -nostdlib -fno-builtin -fno-rtti -fno-exceptions -Wno-unused-value


IO.o: IO.asm
		nasm -f elf -o IO.o IO.asm
	
printf.o: printf.c
	$(CPP) $(CPPFLAGS) -c printf.c -o printf.o
		
Common.o: Common.cpp
	$(CPP) $(CPPFLAGS) -c Common.cpp -o Common.o	
	
Console.o: Console.cpp
	$(CPP) $(CPPFLAGS) -c Console.cpp -o Console.o		
		
IStream.o: IStream.cpp
	$(CPP) $(CPPFLAGS) -c IStream.cpp -o IStream.o
	
OStream.o: OStream.cpp
	$(CPP) $(CPPFLAGS) -c OStream.cpp -o OStream.o				

Util.o: Util.cpp
	$(CPP) $(CPPFLAGS) -c Util.cpp -o Util.o

clean:
	rm -f Common.o
	rm -f Console.o
	rm -f IStream.o
	rm -f OStream.o
	rm -f IO.o
	rm -f printf.o
	rm -f Util.o
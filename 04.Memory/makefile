all: Bitmap.o BitmapFunc.o DescriptorTables.o GDT.o IDT.o InterruptHandler.o MemoryFunc.o MemoryMgr.o PageFunc.o VirtualAlloc.o heap.o malloc.o

Bitmap.o: Bitmap.asm
	nasm -f elf -o Bitmap.o Bitmap.asm

CC		= i686-elf-gcc
CPP		= i686-elf-g++

CFLAGS		= -fno-pic -m32 -Wall -Winline -O3 -fno-keep-inline-functions -fomit-frame-pointer -fstrength-reduce \
		-ffreestanding -fno-exceptions -nostdinc -D $(ARCH)
CPPFLAGS	= -I../include -Wno-comment -Wno-write-strings -Wno-narrowing -Wall -Wpointer-arith -Wcast-align -Wno-unused-parameter -nostdlib -fno-builtin -fno-rtti -fno-exceptions -Wno-unused-value -Wno-unused-but-set-variable

BitmapFunc.o: BitmapFunc.cpp
	$(CPP) $(CPPFLAGS) -c BitmapFunc.cpp -o BitmapFunc.o

DescriptorTables.o: DescriptorTables.s
	nasm -f elf -o DescriptorTables.o DescriptorTables.s
	
GDT.o: GDT.cpp
	$(CPP) $(CPPFLAGS) -c GDT.cpp -o GDT.o

IDT.o: IDT.CPP
	$(CPP) $(CPPFLAGS) -c IDT.cpp -o IDT.o

InterruptHandler.o: InterruptHandler.cpp
	$(CPP) $(CPPFLAGS) -c InterruptHandler.cpp -o InterruptHandler.o
	
MemoryFunc.o: MemoryFunc.asm
	nasm -f elf -o MemoryFunc.o MemoryFunc.asm	
	
MemoryMgr.o: MemoryMgr.cpp
	$(CPP) $(CPPFLAGS) -c MemoryMgr.cpp -o MemoryMgr.o
	
PageFunc.o: PageFunc.c
	$(CPP) $(CPPFLAGS) -c PageFunc.c -o PageFunc.o
	
VirtualAlloc.o: VirtualAlloc.cpp
	$(CPP) $(CPPFLAGS) -c VirtualAlloc.cpp -o VirtualAlloc.o				

malloc.o: malloc.c
	$(CPP) $(CPPFLAGS) -c malloc.c -o malloc.o
	
heap.o: heap.c
	$(CPP) $(CPPFLAGS) -c heap.c -o heap.o	
clean:
	rm -f Bitmap.o
	rm -f BitmapFunc.o
	rm -f DescriptorTables.o
	rm -f GDT.o
	rm -f IDT.o
	rm -f InterruptHandler.o
	rm -f MemoryFunc.o
	rm -f MemoryMgr.o
	rm -f PageFunc.o
	rm -f VirtualAlloc.o
	rm -f malloc.o
	rm -f heap.o
	
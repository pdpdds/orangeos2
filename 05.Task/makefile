all: Task.o TaskManager.o Thread.o TaskSchedule.o

CC		= i686-elf-gcc
CPP		= i686-elf-g++

CFLAGS		= -fno-pic -m32 -Wall -Winline -O3 -fno-keep-inline-functions -fomit-frame-pointer -fstrength-reduce \
		-ffreestanding -fno-exceptions -nostdinc -D $(ARCH)
CPPFLAGS	= -I../include -Wno-comment -Wno-write-strings -Wno-narrowing -Wall -Wpointer-arith -Wcast-align -Wno-unused-parameter -nostdlib -fno-builtin -fno-rtti -fno-exceptions -Wno-unused-value -Wno-unused-but-set-variable -Wno-address-of-packed-member -fno-use-cxa-atexit

Task.o: Task.cpp
	$(CPP) $(CPPFLAGS) -c Task.cpp -o Task.o	

TaskManager.o: TaskManager.cpp
	$(CPP) $(CPPFLAGS) -c TaskManager.cpp -o TaskManager.o
	
Thread.o: Thread.c
	$(CPP) $(CPPFLAGS) -c Thread.c -o Thread.o	

TaskSchedule.o: TaskSchedule.cpp
	$(CPP) $(CPPFLAGS) -c TaskSchedule.cpp -o TaskSchedule.o	


clean:
	rm -f Task.o
	rm -f TaskManager.o
	rm -f Thread.o
	rm -f TaskSchedule.o
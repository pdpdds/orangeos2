all: HardwareSys.o KeyboardController.o pit.o RTC.o

HardwareSys.o: HardwareSys.cpp
	i686-elf-g++ -I../include -fno-use-cxa-atexit -Wno-comment -Wno-write-strings -Wno-narrowing -Wall -Wpointer-arith -Wcast-align -Wno-unused-parameter -nostdlib -fno-builtin -fno-rtti -fno-exceptions -c HardwareSys.cpp -o HardwareSys.o

RTC.o: RTC.c
	i686-elf-g++ -I../include -Wno-comment -Wno-write-strings -Wno-narrowing -Wall -Wpointer-arith -Wcast-align -Wno-unused-parameter -nostdlib -fno-builtin -fno-rtti -fno-exceptions -c RTC.c -o RTC.o

KeyboardController.o: KeyboardController.cpp
	i686-elf-g++ -I../include -Wno-comment -Wno-write-strings  -Wno-narrowing -Wall -Wpointer-arith -Wcast-align -Wno-unused-parameter -nostdlib -fno-builtin -fno-rtti -fno-exceptions -c KeyboardController.cpp -o KeyboardController.o
	
pit.o: pit.cpp
	i686-elf-g++ -I../include -Wno-comment -Wno-write-strings -Wno-narrowing -Wall -Wpointer-arith -Wcast-align -Wno-unused-parameter -nostdlib -fno-builtin -fno-rtti -fno-exceptions -c pit.cpp -o pit.o		

clean:
	rm -f KeyboardController.o
	rm -f pit.o
	rm -f rtc.o
	rm -f HardwareSys.o
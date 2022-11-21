all: Entry.o

Entry.o: Entry.asm
	nasm -f elf -o Entry.o Entry.asm
	
clean:
	rm -f Entry.o
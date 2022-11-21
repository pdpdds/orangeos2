/* Generate an ELF executable */
OUTPUT_FORMAT("elf32-i386")
/* Set the entry point to 'start'. This is an assembly function, so there's
 * no name mangling */
VirtualAddress =  0xC0000000;
PhysicalLoadAddress = 0x100000;
ENTRY(Start)
SECTIONS
{
	/* The executable is to be loaded by GrUB at 1 Mib */
	. = PhysicalLoadAddress;
	/* Align every section on page boundaries. This'll be important later on */
	.text :
	{
		/* This marks the start of the program code */
		code = .;
		/* GrUB requires that the Multiboot header be located at the start
		* of the file */
		*(.Multiboot)
		*(.text*)
		*(.gnu.linkonce.t*)
		*(.gnu.linkonce.r*)
		. = ALIGN( 4096 );    
		codeEnd = .;
	}
	/* This section contains our local variables and constructor
	* (and destructor) lists */
	.data ALIGN(0x1000) :
	{
		data = .;
		/* We've put markers here so that we can iterate in C++ through
		* all of our constructors from firstConstructor to lastConstructor.
		* This list is sorted */
		firstConstructor = .;
		*(SORT(.ctor*))
		lastConstructor = .;
		/* If we were calling destructors, we'd put the markers here. But we aren't,
		* because when we get to multitasking, the code which iterates will never be run.
		*
		* .rodata contains the strings. It's important we keep it here */
		*(.rodata*)
		*(.data*)
		*(.gnu.linkonce.d*)
		dataEnd = .;
	}
	/* There isn't really anything interesting here. It gets zeroed out by GrUB before
	* the kernel is executed */
	.bss ALIGN(0x1000) :
	{
		bss = .;
		*(.gnu.linkonce.b*)
		*(COMMON)
		*(.bss*)
		bssEnd = .;
	}
	end = .;
}
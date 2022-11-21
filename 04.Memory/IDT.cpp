#include "IDT.h"
#include "../03.Library/Common.h"
#include "../03.Library/Util.h"
#include "../02.Hardware/KeyboardController.h"
#include "../02.Hardware/PIT.h"

IDTEntry IDT::idt[256];
DescriptorPointer IDT::idtPointer;

#include "../03.Library/Util.h"
#include "../03.Library/printf.h"

extern void * mm_IDT;
void exceptionHandler(StackState stack)
{
	if(0 == stack.InterruptVector ||
	   12 == stack.InterruptVector ||
	   14  == stack.InterruptVector)
	{

		printf("dfdsfsdf\n");
	}

	outportByte(0x20, 0x20);
}

void irqHandler(StackState stack)
{
	if(32 == stack.InterruptVector)
		PIT::pitHandler(stack);
	else if(33 == stack.InterruptVector)
		KeyboardController::HandleKeyboardInterrupt(stack);

	if(stack.InterruptVector > 39)
		outportByte(0xA0, 0x20);
	outportByte(0x20, 0x20);
}

IDT::IDT()
{
}

IDT::~IDT()
{
}

void IDT::setEntry(unsigned short id, unsigned long functionAddress, unsigned short CS, unsigned char flags)
{
	IDTEntry* pInterrputTable = (IDTEntry*)mm_IDT;

	pInterrputTable[id].FunctionLow = functionAddress & 0xFFFF;
	pInterrputTable[id].CS = CS;
	pInterrputTable[id].Reserved = 0;
	pInterrputTable[id].Flags = flags;
	pInterrputTable[id].FunctionHigh = (functionAddress >> 16) & 0xFFFF;
}

void IDT::SetupIDT()
{

	//Create the basic pointer. It works the same way as the GDT pointer
	idtPointer.Limit = (sizeof(IDTEntry) * 256) - 1;
	idtPointer.Address = (unsigned int)mm_IDT;

	//Provide your own implementation of memset.
	//memset(&idt, 0, sizeof(IDTEntry) * 256);

	//Begin initialisation sequence
	outportByte(0x20, 0x11);	//Primary PIC
	outportByte(0xA0, 0x11);	//Secondary PIC

	//Tell each PIC which IRQs they should handle
	outportByte(0x21, 0x20);
	outportByte(0xA1, 0x28);

	//The primary PIC should communicate with the secondary using interrupt line 2
	outportByte(0x21, 0x4);	//0x4 = 0100 in binary
	//The secondary PIC should communicate with the primary using interrupt line 2
	outportByte(0xA1, 0x2);	//0x2 = 0010 in binary

	//Provide the PICs with some context. Command it to only use the x86 features
	outportByte(0x21, 0x01);
	outportByte(0xA1, 0x01);

	//Let every interrupt get through
	outportByte(0x21, 0x0);
	outportByte(0xA1, 0x0);

	/*
	* This is fairly repetitive. It just sets some fields to the right function pointer.
	* You might find it worthwhile for your own knowledge to look into how you could do this
	* using the ELF symbol tables.
	*/
	setEntry(0, (unsigned int)ISR0, 0x8, 0x8E);
	setEntry(1, (unsigned int)ISR1, 0x8, 0x8E);
	setEntry(2, (unsigned int)ISR2, 0x8, 0x8E);
	setEntry(3, (unsigned int)ISR3, 0x8, 0x8E);
	setEntry(4, (unsigned int)ISR4, 0x8, 0x8E);
	setEntry(5, (unsigned int)ISR5, 0x8, 0x8E);
	setEntry(6, (unsigned int)ISR6, 0x8, 0x8E);
	setEntry(7, (unsigned int)ISR7, 0x8, 0x8E);
	setEntry(8, (unsigned int)ISR8, 0x8, 0x8E);
	setEntry(9, (unsigned int)ISR9, 0x8, 0x8E);
	setEntry(10, (unsigned int)ISR10, 0x8, 0x8E);
	setEntry(11, (unsigned int)ISR11, 0x8, 0x8E);
	setEntry(12, (unsigned int)ISR12, 0x8, 0x8E);
	setEntry(13, (unsigned int)ISR13, 0x8, 0x8E);
	setEntry(14, (unsigned int)ISR14, 0x8, 0x8E);
	setEntry(15, (unsigned int)ISR15, 0x8, 0x8E);
	setEntry(16, (unsigned int)ISR16, 0x8, 0x8E);
	setEntry(17, (unsigned int)ISR17, 0x8, 0x8E);
	setEntry(18, (unsigned int)ISR18, 0x8, 0x8E);
	setEntry(19, (unsigned int)ISR19, 0x8, 0x8E);
	setEntry(20, (unsigned int)ISR20, 0x8, 0x8E);
	setEntry(21, (unsigned int)ISR21, 0x8, 0x8E);
	setEntry(22, (unsigned int)ISR22, 0x8, 0x8E);
	setEntry(23, (unsigned int)ISR23, 0x8, 0x8E);
	setEntry(24, (unsigned int)ISR24, 0x8, 0x8E);
	setEntry(25, (unsigned int)ISR25, 0x8, 0x8E);
	setEntry(26, (unsigned int)ISR26, 0x8, 0x8E);
	setEntry(27, (unsigned int)ISR27, 0x8, 0x8E);
	setEntry(28, (unsigned int)ISR28, 0x8, 0x8E);
	setEntry(29, (unsigned int)ISR29, 0x8, 0x8E);
	setEntry(30, (unsigned int)ISR30, 0x8, 0x8E);
	setEntry(31, (unsigned int)ISR31, 0x8, 0x8E);

	/* 
	* Now, IRQs occupy the same address space, where IRQn is Interrupt[32+n].
	* We alias it so that all this nasty stuff is hidden from most of the OS.
	* Don't forget the binding principle of an OS - modularity. Black-box most of the
	* hardware dependant stuff so that we can change it if need be.
	*/
	setEntry(32, (unsigned int)IRQ0, 0x8, 0x8E);
	setEntry(33, (unsigned int)IRQ1, 0x8, 0x8E);
	setEntry(34, (unsigned int)IRQ2, 0x8, 0x8E);
	setEntry(35, (unsigned int)IRQ3, 0x8, 0x8E);
	setEntry(36, (unsigned int)IRQ4, 0x8, 0x8E);
	setEntry(37, (unsigned int)IRQ5, 0x8, 0x8E);
	setEntry(38, (unsigned int)IRQ6, 0x8, 0x8E);
	setEntry(39, (unsigned int)IRQ7, 0x8, 0x8E);
	setEntry(40, (unsigned int)IRQ8, 0x8, 0x8E);
	setEntry(41, (unsigned int)IRQ9, 0x8, 0x8E);
	setEntry(42, (unsigned int)IRQ10, 0x8, 0x8E);
	setEntry(43, (unsigned int)IRQ11, 0x8, 0x8E);
	setEntry(44, (unsigned int)IRQ12, 0x8, 0x8E);
	setEntry(45, (unsigned int)IRQ13, 0x8, 0x8E);
	setEntry(46, (unsigned int)IRQ14, 0x8, 0x8E);
	setEntry(47, (unsigned int)IRQ15, 0x8, 0x8E);

	//Provide the processor with the IDT.
	//asm volatile ("lidt %0" : "=m"(idtPointer));
}

#ifndef IDT_H
#define IDT_H

#include "../03.Library/commonstruct.h"
#include "DescriptorTables.h"

#define IRQ_KEYBOARD_SERVICE 33

extern "C"
{
	void ISR0(StackState stack);
	void ISR1(StackState stack);
	void ISR2(StackState stack);
	void ISR3(StackState stack);
	void ISR4(StackState stack);
	void ISR5(StackState stack);
	void ISR6(StackState stack);
	void ISR7(StackState stack);
	void ISR8(StackState stack);
	void ISR9(StackState stack);
	void ISR10(StackState stack);
	void ISR11(StackState stack);
	void ISR12(StackState stack);
	void ISR13(StackState stack);
	void ISR14(StackState stack);
	void ISR15(StackState stack);
	void ISR16(StackState stack);
	void ISR17(StackState stack);
	void ISR18(StackState stack);
	void ISR19(StackState stack);
	void ISR20(StackState stack);
	void ISR21(StackState stack);
	void ISR22(StackState stack);
	void ISR23(StackState stack);
	void ISR24(StackState stack);
	void ISR25(StackState stack);
	void ISR26(StackState stack);
	void ISR27(StackState stack);
	void ISR28(StackState stack);
	void ISR29(StackState stack);
	void ISR30(StackState stack);
	void ISR31(StackState stack);

	void IRQ0(StackState stack);
	void IRQ1(StackState stack);
	void IRQ2(StackState stack);
	void IRQ3(StackState stack);
	void IRQ4(StackState stack);
	void IRQ5(StackState stack);
	void IRQ6(StackState stack);
	void IRQ7(StackState stack);
	void IRQ8(StackState stack);
	void IRQ9(StackState stack);
	void IRQ10(StackState stack);
	void IRQ11(StackState stack);
	void IRQ12(StackState stack);
	void IRQ13(StackState stack);
	void IRQ14(StackState stack);
	void IRQ15(StackState stack);

	void exceptionHandler(StackState stack);
	void irqHandler(StackState stack);
}

struct IDTEntry
{
	unsigned short FunctionLow;	//The lower 16 bits of the function address.
	unsigned short CS;			//Code segment offset for the interrupt vector code.
	unsigned char Reserved;
	unsigned char Flags;		//Flags byte. See the tutorial for the full explanation.
	unsigned short FunctionHigh;//The upper 16 bits of the function address.
} __attribute__((packed));

//Just a basic class to set up the IDT
class IDT
{
private:
	IDT();
	~IDT();
	static IDTEntry idt[256];
	static DescriptorPointer idtPointer;
	static void setEntry(unsigned short id, unsigned long functionAddress, unsigned short CS, unsigned char flags);
public:
	static void SetupIDT();
};
#endif

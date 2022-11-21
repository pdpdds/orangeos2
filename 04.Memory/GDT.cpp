#include "DescriptorTables.h"

GDTEntry GDT::gdt[5];
DescriptorPointer GDT::gdtPointer;




void GDT::setEntry(unsigned char id, unsigned short limitLow, unsigned short baseLow, unsigned short baseMiddle, unsigned char access,
		unsigned char granularity, unsigned char baseHigh)
{
	gdt[id].LimitLow = limitLow;
	gdt[id].BaseLow = baseLow;
	gdt[id].BaseMiddle = baseMiddle;
	gdt[id].Access = access;
	gdt[id].Granularity = granularity;
	gdt[id].BaseHigh = baseHigh;
}

void GDT::SetupGDT()
{
	//Null segment
	setEntry(0, 0, 0, 0, 0, 0, 0);
	//Kernel's code segment
	setEntry(1, 0xFFFF, 0, 0, 0x9A, 0xCF, 0);
	//Kernel's data segment
	setEntry(2, 0xFFFF, 0, 0, 0x92, 0xCF, 0);
	//User mode's code segment
	setEntry(3, 0xFFFF, 0, 0, 0xFA, 0xCF, 0);
	//User mode's data segment
	setEntry(4, 0xFFFF, 0, 0, 0xF2, 0xCF, 0);

	gdtPointer.Limit = (sizeof(GDTEntry) * 5) - 1;	//These brackets aren't needed, but make things simpler
	gdtPointer.Address = (unsigned int)(&gdt);

	//Give the processor the GDT pointer
	Processor_SetGDT((unsigned int)(&gdtPointer));
}

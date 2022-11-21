#include "Common.h"

void outportByte(ushort port, uchar value)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

void outportWord(ushort port, ushort value)
{
	asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
}

void outportLong(ushort port, ulong value)
{
	asm volatile ("outl %1, %0" : : "dN" (port), "a" (value));
}

uchar inportByte(ushort port)
{
	uchar result;

	asm volatile ("inb %1, %0" : "=a" (result) : "dN" (port));
	return result;
}

ushort inportWord(ushort port)
{
	ushort result;

	asm volatile ("inw %1, %0" : "=a" (result) : "dN" (port));
	return result;
}

ulong inportLong(ushort port)
{
	ulong result;

	asm volatile ("inl %1, %0" : "=a" (result) : "dN" (port));
	return result;
}

extern "C" void __cxa_pure_virtual()
{
	//This doesn't need to have an implementation. If a virtual call cannot be made, nothing needs to be done
}

//These methods will require an implementation when you implement a memory manager
void *operator new( long unsigned int size)
{
	return (void *)0;
}

void *operator new[]( long unsigned int size)
{
	return (void *)0;
}

void operator delete(void *p)
{
}

void operator delete[](void *p)
{
}

void operator delete(void* p, unsigned long) {
}

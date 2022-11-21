#include "PIT.h"
#include "../03.Library/CONSOLE.h"
#include "../03.Library/Util.h"
#include "../03.Library/printf.h"
#include "../05.Task/TaskSchedule.h"

extern Console OrangeConsole;
unsigned int PIT::ticks;

using namespace Orange;

void PIT::pitHandler(StackState stack)
{
	TaskSchedule::ProcessSchedule();
}

PIT::PIT(unsigned char chan)
{
	channel = chan;
}

PIT::~PIT()
{
}

void PIT::Enable(unsigned int frequency)
{
	unsigned int divisor = TimerFrequency / frequency;

	outportByte(0x43, channel == 0 ? 0x36 : 0xB6);
	outportByte(0x40 + channel, (unsigned char)(divisor & 0xFF));
	outportByte(0x40 + channel, (unsigned char)((divisor >> 8) & 0xFF));
	//AddInterruptHandler(0, pitHandler);
}

void PIT::Disable()
{
	outportByte(0x61, inportByte(0x61) & 0xFC);
}

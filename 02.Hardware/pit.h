#ifndef PIT_H
#define PIT_H

#include "../03.Library/commonstruct.h"
#include "../03.Library/Common.h"

#define TimerFrequency 1193180

class PIT
{
private:
	unsigned char channel;
	unsigned int frequency;
	static unsigned int ticks;
public:
	static void pitHandler(StackState stack);
	PIT(unsigned char chan);
	~PIT();
	void Enable(unsigned int hz);
	void Disable();
};
#endif

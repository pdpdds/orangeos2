/*
 * HardwareSys.cpp
 *
 *  Created on: 2012. 2. 26.
 *      Author: darkx
 */
#include "HardwareSys.h"
#include "KeyboardController.h"
#include "RTC.h"
#include "PIT.h"

PIT PITTimer(0);

HardwareSys::HardwareSys() {
	// TODO Auto-generated constructor stub
}

HardwareSys::~HardwareSys() {
	// TODO Auto-generated destructor stub
}

BOOL HardwareSys::InitializeSystem()
{
	KeyboardController::SetupInterrupts();
	KeyboardController::SetLEDs(true, true, true);

	PITTimer.Enable(50);

	return true;
}

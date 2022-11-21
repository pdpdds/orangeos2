/*
 * HardwareSys.h
 *
 *  Created on: 2012. 2. 26.
 *      Author: darkx
 */

#ifndef HARDWARESYS_H_
#define HARDWARESYS_H_
#include "../03.Library/CommonVar.h"
#include "../03.Library/Common.h"

class HardwareSys {
public:
	HardwareSys();
	virtual ~HardwareSys();

	BOOL InitializeSystem();
};

#endif /* HARDWARESYS_H_ */

/*
 * TaskSchedule.h
 *
 *  Created on: 2012. 3. 2.
 *      Author: darkx3
 */

#ifndef TASKSCHEDULE_H_
#define TASKSCHEDULE_H_
#include "../03.Library/CommonVar.h"
#include "../03.Library/commonstruct.h"
#include "../03.Library/Common.h"

namespace Orange {

class TaskSchedule {
public:
	TaskSchedule();
	virtual ~TaskSchedule();

	static BOOL ProcessSchedule();

protected:

private:
};

} /* namespace Orange */
#endif /* TASKSCHEDULE_H_ */

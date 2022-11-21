/*
 * Task.cpp
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#include "Task.h"

namespace Orange {

Task::Task()
{
	// TODO Auto-generated constructor stub

}

Task::~Task() {
	// TODO Auto-generated destructor stub
}



void Task::SetPageBitmap(void* pPageBitmap)
{
	m_TaskInfo.pPageBitmap = (BYTE*)pPageBitmap;
}

} /* namespace Orange */

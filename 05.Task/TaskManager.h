/*
 * TaskManager.h
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_
#include "Task.h"
#include "../03.Library/commonstruct.h"
#include "../03.Library/Common.h"
#include "../06.DataStructure/List.h"
#include "Thread.h"

#define TASK_INITIAL_HEAP_SIZE  1024
#define TASK_MAX_HEAP_SIZE      (1024 * 1024)


#define TASK_PAGE_BITMAP_SIZE 32         //size of the pagebitmap in pages

#define KERNEL_CODE_SELECTOR 0x8
#define KERNEL_DATA_SELECTOR 0x10

#define TASK_CODE_SELECTOR 0x19
#define TASK_DATA_SELECTOR 0x21

#define TASK_FLAG_VALUE 0x202 //0x3202

#define TASK_PRIORITY_LEVELS 7
#define TASK_PRIORITY_RATIO  5          //0 - no priority

#define TP_TIME_CRITICAL 0
#define TP_HIGHEST       1
#define TP_ABOVE_NORMAL  2
#define TP_NORMAL        3
#define TP_BELOW_NORMAL  4
#define TP_LOWEST        5
#define TP_ABOVE_IDLE    6
#define TP_IDLE          7

#define TASK_MEM_PROTECTION     3

#define CUR_DIR_SIZE     500

namespace Orange {

class Task;

class TaskManager {
public:
	TaskManager();
	virtual ~TaskManager();

	BOOL Initialize();
	static UINT32 GetNextTaskID(){return m_NextTaskID++;}

	static Task* GetSystemTask(){return &m_SystemTask;}

	Task* GetTask(UINT32 TaskID);
	LPLISTNODE GetTaskList(){return m_pTaskList;}
	UINT32 RemoveTask(UINT32 TaskID);
	Task* CreateMemoryTask(void(*lpThreadAddress)());

	Task* GetCurrentTask(){return m_pCurrentTask;}
	LPTHREAD GetCurrentThread(){return m_pCurrentThread;}
	void SetCurrentThread(LPTHREAD pCurrentThread){m_pCurrentThread = pCurrentThread;}

	TaskStateSegment* GetTSS(){return m_pTSS;}

protected:
	BOOL SetTSS(TaskStateSegment* pTSS);

private:
	static UINT32 m_NextTaskID;
	static Task m_SystemTask;

	Task* m_pCurrentTask;
	LPLISTNODE m_pTaskList;

	TaskStateSegment* m_pTSS;
	LPTHREAD m_pCurrentThread;
};

} /* namespace Orange */
#endif /* TASKMANAGER_H_ */

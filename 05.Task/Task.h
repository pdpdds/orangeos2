/*
 * Task.h
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#ifndef TASK_H_
#define TASK_H_
#include "../03.Library/commonstruct.h"
#include "../03.Library/Common.h"
#include "TaskDescriptor.h"
#include "../06.DataStructure/List.h"

namespace Orange {

struct TaskInfo
{
    //LPOBJECT lpObject;

    UINT32 TaskID;
    UINT32 dwRunState;
    UINT32 dwPriority;
    UINT32 dwWaitingTime;

    void * pPageDirectory;
    BYTE * pPageBitmap;
    TaskStateSegment* pTSS;

    LPHEAPINFO lpHeapInfo;

    LPLISTNODE ThreadQueue;

   // LPLISTNODE PEExportList;     //the symbols exported by this task

   // LPLISTNODE EnvironmentVariables;

   // char szCurrentDirectory[CUR_DIR_SIZE];
}__attribute__ ((packed));

class Task {
public:
	Task();
	virtual ~Task();

	UINT32 GetTaskID(){return m_TaskInfo.TaskID;}
	void SetTaskID(UINT32 TaskID){m_TaskInfo.TaskID = TaskID;}

	BYTE* GetPageBitmap(){return m_TaskInfo.pPageBitmap;}
	void SetPageBitmap(void* pPageBitmap);//{m_TaskInfo.m_pPageBitmap = (BYTE*)pPageBitmap;}

	void* GetPageDirectory(){return m_TaskInfo.pPageDirectory;}
	void SetPageDirectory(void* pPageDirectory){m_TaskInfo.pPageDirectory = pPageDirectory;}

	TaskInfo* GetTaskInfo(){return &m_TaskInfo;}

protected:

private:
	TaskInfo m_TaskInfo;
};

} /* namespace Orange */
#endif /* TASK_H_ */

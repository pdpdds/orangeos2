/*
 * TaskSchedule.cpp
 *
 *  Created on: 2012. 3. 2.
 *      Author: darkx3
 */

#include "TaskSchedule.h"
#include "TaskManager.h"
#include "../04.Memory/DescriptorTables.h"
#include "../04.Memory/PageFunc.h"

extern Orange::TaskManager TaskMgr;
extern void * mm_KernelPageDirectory;
extern UINT32 sys_IOBasePhysicalAddress;

namespace Orange {

TaskSchedule::TaskSchedule() {
	// TODO Auto-generated constructor stub

}

TaskSchedule::~TaskSchedule() {
	// TODO Auto-generated destructor stub
}

BOOL TaskSchedule::ProcessSchedule()
{
	LPLISTNODE pTaskNode = TaskMgr.GetTaskList();
	Task* pTask = NULL;
	Task* pSelectedTask = NULL;

	while(pTaskNode != NULL)
	{
		pTask = (Task*)pTaskNode->ptrData;


		pTask->GetTaskInfo()->dwWaitingTime++;

		if(NULL == pSelectedTask)
		{
			pSelectedTask = pTask;
		}
		else
		{
			if(pTask != TaskMgr.GetCurrentTask())
			{

				if (pSelectedTask->GetTaskInfo()->dwWaitingTime < pTask->GetTaskInfo()->dwWaitingTime)

				{

					pSelectedTask = pTask;

				}

			}
		}

		pTaskNode = pTaskNode->Next;
	}


	LPTHREAD pSelectedThread = NULL;

	if(pSelectedTask && pSelectedTask != TaskMgr.GetCurrentTask())
	{
		LPLISTNODE pThreadNode = pSelectedTask->GetTaskInfo()->ThreadQueue;

		while(NULL != pThreadNode)
		{
			 LPTHREAD hNextThread=(LPTHREAD)pThreadNode->ptrData;


			 if(hNextThread == NULL)
			 {
				 break;
			 }
			 else
			 {
				 hNextThread->dwWaitingTime++;

				 if(pSelectedThread == NULL)
				 {
					 pSelectedThread = hNextThread;
					 pThreadNode = pThreadNode->Next;
					 continue;
				 }


				 if(pSelectedThread == hNextThread)
				 {
					 pThreadNode = pThreadNode->Next;
					 continue;
				 }

				 if (pSelectedThread->dwWaitingTime < hNextThread->dwWaitingTime)
				 {
					 pSelectedThread = hNextThread;
				 }
			 }

			 pThreadNode = pThreadNode->Next;
		}
	}

	if(pSelectedThread != NULL && pSelectedThread != TaskMgr.GetCurrentThread())
	{
		TaskMgr.SetCurrentThread(pSelectedThread);
		TaskStateSegment* sys_TSS = TaskMgr.GetTSS();

		TaskMgr.GetTSS()->SS0= pSelectedThread->StackInfo.SS0;
		TaskMgr.GetTSS()->SS1=pSelectedThread->StackInfo.SS1;
		TaskMgr.GetTSS()->SS2=pSelectedThread->StackInfo.SS2;

		TaskMgr.GetTSS()->ESP0=pSelectedThread->StackInfo.ESP0;
		TaskMgr.GetTSS()->ESP1=pSelectedThread->StackInfo.ESP1;
		TaskMgr.GetTSS()->ESP2=pSelectedThread->StackInfo.ESP2;

		      //      #ifdef _DEBUG_
		        //        dbgprintf("\n New Thread = %X sysCurThreadStackInfo %X", sys_CurThread, sysCurThreadStackInfo );
		          //      dbgprintf("\n SS0 %X:%X SS1 %X:%X SS2 %X:%X",sys_TSS->SS0,sys_TSS->ESP0,sys_TSS->SS1,sys_TSS->ESP1,sys_TSS->SS2,sys_TSS->ESP2);
		            //#endif

		if ( pSelectedThread->IOBASE != NULL ) //if IO Bitmap is exists then map physical address of IO bitmap to the TSS->IOBase linear addresss
		            {
		               // DEBUG_PRINT_OBJECT1("Setting Customized IOBASE %X", lpThread->IOBASE );
		                SetPageEntry( ((UINT32 )( ((UINT32)sys_TSS)+sys_TSS->IOBASE))/VASM_PAGE_SIZE,    mm_KernelPageDirectory, (UINT32*)VASM_TPT, (void *) GetPageEntry((UINT32)pSelectedThread->IOBASE/VASM_PAGE_SIZE,     (UINT32*)VASM_TPT )  , TASK_MEM_PROTECTION);
		                SetPageEntry( (((UINT32)( ((UINT32)sys_TSS)+sys_TSS->IOBASE))/VASM_PAGE_SIZE)+1, mm_KernelPageDirectory, (UINT32*)VASM_TPT, (void *) GetPageEntry(((UINT32)pSelectedThread->IOBASE/VASM_PAGE_SIZE)+1, (UINT32*)VASM_TPT )  , TASK_MEM_PROTECTION);
		                //DEBUG_PRINT_OBJECT2("lpThread->IOBASE - %X sys_TSS->IOBASE %X", lpThread->IOBASE, sys_TSS->IOBASE);

		            }
		            else            //if IO Bitmap not present then map the system default IO bitmap
		            {
		               // DEBUG_PRINT_OBJECT2("lpThread->IOBASE is NULL - Setting Default IOBASE sys_TSS %X -> IOBase %X", sys_TSS, sys_TSS->IOBASE);
		                SetPageEntry( ((UINT32) (((UINT32)sys_TSS)+sys_TSS->IOBASE))/VASM_PAGE_SIZE,    mm_KernelPageDirectory, (UINT32*)VASM_TPT, (void *) sys_IOBasePhysicalAddress  , TASK_MEM_PROTECTION);
		                SetPageEntry( (((UINT32)(((UINT32)sys_TSS)+sys_TSS->IOBASE))/VASM_PAGE_SIZE)+1, mm_KernelPageDirectory, (UINT32*)VASM_TPT, (void *) ((UINT32)sys_IOBasePhysicalAddress+VASM_PAGE_SIZE), TASK_MEM_PROTECTION);
		            }
		            //invalidate the IOBitmap pages
		            __asm__ __volatile__ ("invlpg (%0)": :"r" (((UINT32)sys_TSS)+sys_TSS->IOBASE));
		            __asm__ __volatile__ ("invlpg (%0)": :"r" (((UINT32)sys_TSS)+sys_TSS->IOBASE+ VASM_PAGE_SIZE));
	}

	return TRUE;
}

} /* namespace Orange */

/*
 * TaskManager.cpp
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#include "TaskManager.h"
#include "Task.h"

#include "../03.Library/COMMONVAR.h"
#include "../03.Library/COMMON.h"
#include "../03.Library/printf.h"
#include "../04.Memory/VirtualAlloc.h"
#include "../04.Memory/Heap.h"
#include "../04.Memory/DescriptorTables.h"
#include "../04.Memory/PageFunc.h"
#include "../04.Memory/malloc.h"

using namespace Orange;

extern void * mm_KernelPageBitmap;                   //Kernel's Page Bitmap intalized during Phase I of InitMem
extern void * mm_KernelPageDirectory;                  //Kernel Page Directory intalized during Phase I of InitMem
UINT32 sys_IOBasePhysicalAddress;

/*The following function will be called when the system is becoming idle. The hlt instruction makes the CPU to stop working*/
void IdleLoop()
{
    asm volatile("loop: \
                    hlt; \
                    jmp loop");
}

namespace Orange {

UINT32 TaskManager::m_NextTaskID = 0;
Task TaskManager::m_SystemTask;


TaskManager::TaskManager() {
	// TODO Auto-generated constructor stub
	m_pTSS = NULL;
	m_pCurrentThread = NULL;
	m_pCurrentTask = NULL;
}

TaskManager::~TaskManager() {
	// TODO Auto-generated destructor stub
}

//1 page for TSS contents and 2 pages for IOBase bitmap (65535 ports)
#define TASK_TSS_PAGES          3

#define TASK_STACK_SIZE     6000
#define TASK_MEM_PROTECTION     3

#define INITIAL_KERNEL_HEAP 2000

LISTNODE InitNode;

BOOL TaskManager::Initialize()
{
	printf("Task Manager Initialize...\n");
	m_SystemTask.SetTaskID(GetNextTaskID());
	m_SystemTask.GetTaskInfo()->pPageBitmap = (BYTE*)mm_KernelPageBitmap;
	m_SystemTask.GetTaskInfo()->pPageDirectory = mm_KernelPageDirectory;

	InitNode.Next = NULL;
	InitNode.ptrData = (void *)(&m_SystemTask);
	InitNode.szKey[0] = 0;

	m_pTaskList = &InitNode;

	//any heap creation should after TaskList creation and TaskId assignment
	m_SystemTask.GetTaskInfo()->lpHeapInfo = HeapCreateEx(0, 0, INITIAL_KERNEL_HEAP, 0 ); //create kernel heap

	if(m_SystemTask.GetTaskInfo()->lpHeapInfo == NULL)
	{
		printf("Heap Create Fail...\n");
		while(1)
		{

		}
	}

	m_pTSS = (TaskStateSegment *)VirtualAllocEx(m_SystemTask.GetTaskID(), 0, TASK_TSS_PAGES, MEM_RESERVE|MEM_COMMIT, TASK_MEM_PROTECTION); //1 page for TSS contents and 2 pages for IOBase bitmap (65535 ports)

	if(m_pTSS == NULL)
	{
		printf("TSS Alloc Fail...\n");
		while(1)
		{

		}
	}

	SetTSS(m_pTSS);


	//create kernel thread

	LPTHREAD lpThread = (LPTHREAD)malloc(sizeof(THREAD) );

	if ( lpThread == NULL )
	{
//	        DEBUG_PRINT_INFO("Kernel thread creation failed.");
	        //here the error will be set by the CreateThread()

		return FALSE;
	}

	sys_CurThread = lpThread;
	lpThread->dwRunState = TASK_RUNNABLE;//ready and not running
	lpThread->dwPriority = TP_NORMAL;
	lpThread->dwWaitingTime = 0;


	   // lpThread->lpMessageBox = IPC_CreateMessageBox(DEFAULT_THREAD_MESSAGES);
	lpThread->StackInfo.ESP0 = (UINT32) malloc(TASK_STACK_SIZE); //allocate stack
	lpThread->StackInfo.ESP1 = 0;
	lpThread->StackInfo.ESP2 = 0;
	lpThread->StackInfo.SS0 = KERNEL_DATA_SELECTOR;
	lpThread->StackInfo.SS1 = 0;

	lpThread->IOBASE = NULL;
	    //adding the base thread to the thread queue
	List_Add(&(m_SystemTask.GetTaskInfo()->ThreadQueue),"",lpThread);

	    //creating Idle thread
	m_pCurrentThread = CreateThreadEx(0, IdleLoop, TASK_STACK_SIZE, KERNEL_CODE_SELECTOR, KERNEL_DATA_SELECTOR, TASK_FLAG_VALUE, TASK_RUNNABLE, TP_IDLE, FALSE, FALSE);

    Task* pMemoryThread = CreateMemoryTask(IdleLoop);

    if(pMemoryThread == NULL)
    {
    	while(1)
    	{

    	}
    }

    m_pCurrentTask = &m_SystemTask;

	return TRUE;
}

Task* TaskManager::GetTask(UINT32 TaskID)
{
    UINT32 TotTasks = List_Count(m_pTaskList);
    UINT32 i = 0;
    for(i=0;i<TotTasks;i++)     // Search througth tasks to get task memory info
    {
        Task * pTask =(Task*)List_GetData(m_pTaskList,"",i);

        if ( pTask != NULL )
            if( pTask->GetTaskID() == TaskID )
            {
                //SetLastError( ERROR_SUCCESS );
                return pTask;
            }
    }
  //  SetLastError( ERROR_INVALID_HANDLE );
    return NULL;
}

/* This function removes the task from the system task list*/
UINT32 TaskManager::RemoveTask(UINT32 TaskID)
{
    UINT32 TotTasks = List_Count(m_pTaskList);
    UINT32 i = 0;
    for(i=0;i<TotTasks;i++)     // Search througth tasks to get task memory info
    {
        Task* pTask=(Task*)List_GetData(m_pTaskList,"",i);
        if ( pTask != NULL )
            if( pTask->GetTaskID() == TaskID )
            {
                List_Delete(&m_pTaskList,"",i);
                //clear task, thread, memory everything only after confirming that no thread of that task is in RUNNING state.
                //free( TI );

                //SetLastError( ERROR_SUCCESS );
                return 1;
            }
    }
    //SetLastError( ERROR_INVALID_HANDLE );
    return 0;
}

BOOL TaskManager::SetTSS(TaskStateSegment* pTSS) //TSS는 1페이지의 크기 = 4096 BYTE
{
	//filling the Kernel TSS with zero
	 UINT32 i = 0;
	for(i=0; i < VASM_PAGE_SIZE/4; i++)
		((UINT32 *)pTSS)[i]=0;

	pTSS->IOBASE = VASM_PAGE_SIZE;//+32;     //system IO Bitmap base address from sys_TSS base + Interrupt redirection bitmap

	sys_IOBasePhysicalAddress=GetPageEntry( (((UINT32)pTSS)/VASM_PAGE_SIZE)+1, (UINT32*)VASM_TPT );
		    //all Interrupts are redirected to V86
		    for(i=0;i<8;i++)
		        ((UINT32*)(pTSS))[(VASM_PAGE_SIZE/4)+i]=0;
		    //by default access to all IO ports denied for device drivers and applications
		    for(i=8;i<VASM_PAGE_SIZE/2;i++)
		        ((UINT32*)(pTSS))[(VASM_PAGE_SIZE/4)+i]=0xFFFFFFFF;   //all are 1s

		    pTSS->SS0 = KERNEL_DATA_SELECTOR;

		    //loading the Kernel Page Directory Address
		    pTSS->CR3=((UINT32)GetPageEntry( (UINT32)mm_KernelPageDirectory/VASM_PAGE_SIZE, (UINT32*)VASM_TPT  ) & 0xFFFFF000) | 3;

		    //creating Task State Segment Selector
		    UINT16 TSSSelector = CreateGDTEntry( pTSS, TASK_TSS_PAGES*VASM_PAGE_SIZE, 0x80, 0xE9 );
		    if ( TSSSelector == 0 )
		    {
		       // DEBUG_PRINT_INFO("Unable to create system TSS Seletor");
		       // SetLastError( ERROR_NOT_ENOUGH_MEMORY );
		        return FALSE;
		    }
		    //just loads the Kernel TSS into memory **ltr instruction
		    asm volatile("ltr %0": :"m"(TSSSelector));

	return TRUE;
}

Task* TaskManager::CreateMemoryTask(void(*lpThreadAddress)()) //파일에서 프로세스를 생성하지 않고 메모리에 존재하는 함수로 부터 생성을 한다.
{
	Task* lpTask = NULL;
	lpThreadAddress = 0;
    ALLOC_AND_CHECK(lpTask, Task*, sizeof(Task), NULL)

    if ( List_Add( &m_pTaskList, "", lpTask ) )
    {
        // DEBUG_PRINT_OBJECT1("Adding task to the sys_TaskList %X failed", &sys_TaskList);
    }
     //DEBUG_PRINT_OBJECT1("Returns %x",lpTask);

        lpTask->SetTaskID(GetNextTaskID());
        lpTask->GetTaskInfo()->dwRunState = TASK_RUNNABLE;
        lpTask->GetTaskInfo()->dwPriority = TP_NORMAL;
        lpTask->GetTaskInfo()->dwWaitingTime = 0;
      //  #warning check the below assignment
        lpTask->GetTaskInfo()->pPageDirectory = GetSystemTask()->GetPageDirectory();
        lpTask->GetTaskInfo()->pPageBitmap = GetSystemTask()->GetPageBitmap();
        lpTask->GetTaskInfo()->lpHeapInfo = HeapCreateEx(lpTask->GetTaskID(), 0, TASK_INITIAL_HEAP_SIZE, TASK_MAX_HEAP_SIZE);
        lpTask->GetTaskInfo()->ThreadQueue = NULL;
        //lpTask->PEExportList = NULL;
        //lpTask->EnvironmentVariables = NULL;

        //DEBUG_PRINT_OBJECT1("Creating Thread for PI.AddressOfEntryPoint %X", PI.lpLoadedAddress + PI.AddressOfEntryPoint);
        LPTHREAD lpThread = CreateThreadEx(lpTask, lpThreadAddress, TASK_STACK_SIZE, TASK_CODE_SELECTOR, TASK_DATA_SELECTOR, TASK_FLAG_VALUE, TASK_RUNNABLE, TP_NORMAL, FALSE, FALSE);
        if ( lpThread == NULL )
        {
        	//printf("list add failaaaaaa\n");
           // DEBUG_PRINT_INFO("Thread creation failed.");
            return NULL;
        }
        if ( List_Add( &lpTask->GetTaskInfo()->ThreadQueue, "", lpThread) )
        {

          //  DEBUG_PRINT_OBJECT1("Adding thread to the list %X failed", &lpTask->ThreadQueue);
            return NULL;
        }
        printf("returns  NULL.%x\n", lpTask->GetTaskInfo()->pPageDirectory);

    return lpTask;
}

} /* namespace Orange */

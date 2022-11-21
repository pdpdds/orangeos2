#include "malloc.h"
#include "Heap.h"
#include "../05.Task/TaskManager.h"
#include "../05.Task/Task.h"

extern Orange::TaskManager TaskMgr;

using namespace Orange;

void * malloc(size_t NoOfBytes)
{
    Task* lpTask= TaskMgr.GetTask(0);
    if ( lpTask == NULL )
        return NULL;
    return HeapAlloc( 0, (HeapInfo*)(lpTask->GetTaskInfo()->lpHeapInfo), NoOfBytes);
}

void * free(void * pObject)
{
    Task* lpTask = TaskMgr.GetTask(0);
    if ( lpTask == NULL )
        return NULL;
    
    if ( HeapFree( lpTask->GetTaskInfo()->lpHeapInfo, pObject) )
        return pObject;
    else
        return NULL;
}


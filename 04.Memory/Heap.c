/*
	Heap Memory Manager for Ace OS Kernel / Applications
			- Created by Samuel on 08-Aug-2002 (samuelhard@yahoo.com)
*/

#include "Heap.h"
#include "../03.Library/printf.h"
#include "../03.Library/UTIL.h"
#include "VirtualAlloc.h"
#include "DescriptorTables.h"
#include "../05.Task/Task.h"
//#define DEBUG

using namespace Orange;

#define PADSIZE 0  

#ifdef DEBUG
    UINT32 DbgAllocated=0,DbgFreed=0,DbgAllocPages=0;
#endif
void DumpHeapInfo(LPHEAP Root )
{
    printf("\n HEAP :: Dump - ");
    while( Root )
    {
        printf("[%x]",Root);
        printf("%s(%ld)",Root->IsUsed?"*":"",Root->dwSize );
        Root=Root->Next;
    }
    printf(" $$ End");
}
LPHEAPINFO HeapCreateEx(UINT32 TaskID, UINT32 dwOptions, UINT32 dwInitialSize, UINT32 dwMaximumSize)
{
    UINT32 dwReqPages=(dwInitialSize+sizeof(HEAPINFO))/VASM_PAGE_SIZE+1;
    #ifdef DEBUG
        printf("\n HEAP :: Create(%X,%ld,%ld)", dwOptions,dwInitialSize, dwMaximumSize );
    #endif
    LPHEAPINFO hHeapInfo=(LPHEAPINFO)VirtualAllocEx(TaskID, 0, dwReqPages , MEM_RESERVE | MEM_COMMIT, 0 );
    if ( hHeapInfo == NULL )
        return NULL;
    #ifdef DEBUG
        printf("\n\t VirtualAlloc() :: %X", hHeapInfo );
    #endif
    LPHEAP hHeapRoot=(LPHEAP)( (UINT32)hHeapInfo + sizeof(HEAPINFO) );
    hHeapRoot->Next=hHeapRoot->Previous=NULL;
    hHeapRoot->IsUsed=FALSE;
    hHeapRoot->dwSize=(dwReqPages * VASM_PAGE_SIZE)-sizeof(HEAPINFO);
    
    hHeapInfo->hHeapRoot=hHeapRoot;
    hHeapInfo->dwOptions=dwOptions;
    hHeapInfo->dwCurrentSize=(dwReqPages * VASM_PAGE_SIZE)-sizeof(HEAPINFO);
    hHeapInfo->dwMaximumSize=dwMaximumSize;
    return hHeapInfo;
}
//Allocate Heap memory to process
void * HeapAlloc(UINT32 TaskID, LPHEAPINFO hHeapInfo, UINT32 dwSize)
{
    dwSize+=PADSIZE;
    #ifdef DEBUG
        printf("\n\r HEAP :: Alloc(%X,%ld) Allocated=%ld Pages=%ld Freed=%d HEAPSIZE=%d",hHeapInfo,dwSize, DbgAllocated,DbgAllocPages,DbgFreed, sizeof(HEAP) );
    #endif
    if ( hHeapInfo == NULL )    //invalid Heap Info
        return NULL;
    if ( dwSize <= 0 )          //invalid size
        return NULL;
    LPHEAP hHeap = hHeapInfo->hHeapRoot;
    #ifdef DEBUG
        DumpHeapInfo( hHeap );
        printf("\n HEAP :: Free Node Search with size [%ld] ",dwSize);
    #endif
    while( hHeap )
    {
        #ifdef DEBUG
            printf("[%X%s(%ld)]->",hHeap, hHeap->IsUsed?"*":"" ,(UINT32) hHeap->dwSize);
        #endif
        if ( !hHeap->IsUsed && hHeap->dwSize >= dwSize + sizeof(HEAP) )
            break;
        hHeap=hHeap->Next;
    }
    if ( hHeap == NULL ) //no free node is found with specified size
    {
        #ifdef DEBUG
            printf("\n HEAP :: Allocating Virtual %ld Pages ...",(UINT32) (dwSize/VASM_PAGE_SIZE) + 1);
        #endif
        LPHEAP hNewHeap=(LPHEAP)VirtualAllocEx(TaskID, 0, (dwSize/VASM_PAGE_SIZE)  + 1 , MEM_RESERVE | MEM_COMMIT, 0 );
        LPHEAP hOldHeap=hHeapInfo->hHeapRoot;
        
        if ( hNewHeap == NULL )
        {
            printf(" Virtual Alloc Failed.");
            return NULL;
        }
        #ifdef DEBUG
            printf("done. Starting VAS %X",hNewHeap);
            DbgAllocPages+=dwSize/VASM_PAGE_SIZE  + 1 ;
        #endif
        hNewHeap->dwSize=( ( (dwSize/VASM_PAGE_SIZE ) + 1 ) * VASM_PAGE_SIZE  )- sizeof( HEAP );
        hNewHeap->IsUsed=FALSE;
        hHeapInfo->hHeapRoot=hNewHeap;
        hNewHeap->Next = hOldHeap;
        hNewHeap->Previous = NULL;
        if ( hOldHeap != NULL )
            hOldHeap->Previous = hNewHeap;
        
        hHeap = hNewHeap;
        #ifdef DEBUG
            printf("\n HEAP :: hNewHeap->dwSize=%ld hNewHeap->Next=%X",hNewHeap->dwSize, hNewHeap->Next);
            DumpHeapInfo( hHeapInfo->hHeapRoot );
        #endif
        
    }
    #ifdef DEBUG
        printf("\n HEAP :: Required Heap Node %X (%ld)",hHeap, (UINT32)hHeap->dwSize );
    #endif
    
    if ( hHeap->dwSize > dwSize + sizeof(HEAP)*2 ) // if the required node contains more space for spliting it to two nodes
    {
        #ifdef DEBUG
            printf("\n HEAP :: Spliting .. ");
        #endif
        #ifdef DEBUG
            printf(" hFreeHeap -");
        #endif
        LPHEAP hFreeHeap = hHeap; // creating a free node based on the excess space on the current node
        hFreeHeap->dwSize = hFreeHeap->dwSize -( dwSize + sizeof(HEAP)*2 ); // excess size 
        hFreeHeap->IsUsed =FALSE; //free
        
        #ifdef DEBUG
            printf(" %X(%ld)", hFreeHeap,hFreeHeap->dwSize);
            printf(" ReqNode -");
        #endif
        //the required node is placed after the excess space
        hHeap = (HEAP *) ((UINT32)hFreeHeap + sizeof(HEAP) + hFreeHeap->dwSize );
        hHeap->dwSize=dwSize;          //required size
        hHeap->IsUsed=TRUE;       //occupied
        hHeap->Next = hFreeHeap->Next; 
        hHeap->Previous = hFreeHeap;
        #ifdef DEBUG
            printf(" %X(%d)", hHeap,hHeap->dwSize);
        #endif
        hFreeHeap->Next = hHeap; 
        
        #ifdef DEBUG
            printf("Free Node %X(%ld) hHeap %X(%ld)",hFreeHeap,hFreeHeap->dwSize, hHeap, hHeap->dwSize );
        #endif
    }
    else
    {
        #ifdef DEBUG
            printf("\n HEAP :: Found a node with equal size %X(%d)=%d", hHeap, hHeap->dwSize, dwSize + sizeof(HEAP));
        #endif
        hHeap->IsUsed = TRUE;
    }
    #ifdef DEBUG
        printf("\n HEAP :: Return %X",(HEAP *)((UINT32)hHeap+sizeof(HEAP)) );
        DbgAllocated+=dwSize;
    #endif
    return (HEAP *)((UINT32)hHeap+sizeof(HEAP));
}

//Deallocates the already allocated memory and retains the address for future use
BYTE HeapFree(LPHEAPINFO hHeapInfo, void * lpAddress)
{
    #ifdef DEBUG
        dbgprintf("\n\r HEAP :: Free(%X,%X)",hHeapInfo,lpAddress);
    #endif
    LPHEAP hHeap = hHeapInfo->hHeapRoot;
    
    while( hHeap )
    {
        #ifdef DEBUG
            dbgprintf("[%X%s]->",hHeap, hHeap->IsUsed?"*":"");
        #endif
        if ( !hHeap->IsUsed && (LPHEAP) ((UINT32)hHeap+sizeof(HEAP)) == (LPHEAP)lpAddress )
            break;
        hHeap=hHeap->Next;
        
    }
    if ( hHeap == NULL )
        return 0; // address specified not found or already freed
    
    hHeap->IsUsed=FALSE;
    return 1;
}

void *HeapRealloc(LPHEAPINFO lpHeapInfo, void * lpAddress, UINT32 dwSize)
{
    void * lpNewMem = (void *)HeapAlloc(0, lpHeapInfo, dwSize);
    if ( lpNewMem == NULL )
        return NULL;
    LPHEAP lpOldHeap = lpHeapInfo->hHeapRoot;
    while( lpOldHeap )
    {
        if ( (LPHEAP) ((UINT32)lpOldHeap+sizeof(HEAP)) == (LPHEAP)lpAddress )
            break;
        lpOldHeap=lpOldHeap->Next;
    }
    if ( lpOldHeap )
    {
        memcpy( lpNewMem, lpAddress, sizeof(lpOldHeap) );//이상함 분석 요망
        HeapFree( lpHeapInfo, lpAddress );
        return lpNewMem;
    }
    printf("\n HeapRealloc Failure ");
    return NULL;
}
/*
void * GlobalAlloc(UINT32 dwSize)
{
    printf("\nGlobalAlloc called with %d", dwSize);
    Task* lpTask = GetTaskInfo( GetTaskID() );
    if ( lpTask == NULL )
        return NULL;
    LPHEAPINFO lpHeapInfo = lpTask->lpHeapInfo;
    if ( lpHeapInfo == NULL )
        return NULL;
    return HeapAlloc( lpHeapInfo, dwSize );
}
void * GlobalRealloc(void * lpAddress, UINT32 dwSize)
{
    Task* lpTask = GetTaskInfo( GetTaskID() );
    if ( lpTask == NULL )
        return NULL;
    LPHEAPINFO lpHeapInfo = lpTask->lpHeapInfo;
    if ( lpHeapInfo == NULL )
        return NULL;
    return HeapRealloc( lpHeapInfo, lpAddress, dwSize );
}
void * GlobalFree(void * lpAddress)
{
    Task* lpTask = GetTaskInfo( GetTaskID() );
    if ( lpTask == NULL )
        return NULL;
    LPHEAPINFO lpHeapInfo = lpTask->lpHeapInfo;
    if ( lpHeapInfo == NULL )
        return NULL;

    if ( HeapFree( lpHeapInfo, lpAddress ) )
        return NULL;
    else
        return lpAddress;
}*/



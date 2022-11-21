/*---------------------Ace OS source Code-----------------------
		Created by Sam on 11-Aug-2002(samuelhard@yahoo.com)
 This file is a header file for Heap manager
--------------------------------------------------------------*/
#ifndef HEAP_H
#define HEAP_H
#include "../03.Library/COMMONVAR.h"
#include "../03.Library/COMMON.h"
#include "../03.Library/CommonStruct.h"

#ifdef __cplusplus
	extern "C" {
#endif

LPHEAPINFO HeapCreateEx(UINT32 TaskID, UINT32 dwOptions, UINT32 dwInitialSize, UINT32 dwMaximumSize);
void * HeapAlloc(UINT32 TaskID, LPHEAPINFO hHeapInfo, UINT32 dwSize);
void *HeapRealloc(LPHEAPINFO lpHeapInfo, void * lpAddress, UINT32 dwsize);
BYTE HeapFree(LPHEAPINFO hHeapInfo, void * lpAddress);

//void * GlobalAlloc(UINT32 dwSize);
//void * GlobalRealloc(void * lpAddress, UINT32 dwSize);
//void * GlobalFree(void * lpAddress);

#ifdef __cplusplus
	}
#endif

#endif

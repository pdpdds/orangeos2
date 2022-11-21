/*
 * MemoryMgr.h
 *
 *  Created on: 2012. 2. 26.
 *      Author: darkx
 */

#ifndef MEMORYMGR_H_
#define MEMORYMGR_H_
#include "../03.Library/COMMONVAR.h"
#include "../03.Library/COMMON.h"

extern "C" DWORD DetectMemory();

void * KernelUnMap(void * lpStartingAddress, UINT32 Size);

void SetUpMemory();


extern "C" void* InitKernelWorkingMemory(multiboot_header_t* pBoot,  multibootInfo *mbootPtr);

#endif /* MEMORYMGR_H_ */

/*
 * CommonStruct.h
 *
 *  Created on: 2012. 2. 23.
 *      Author: darkx
 */

#ifndef COMMONSTRUCT_H_
#define COMMONSTRUCT_H_
#include "CommonVar.h"

struct StackState
{
	unsigned int DS;											//Data segment selector
	unsigned int EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX;		//Pushed by pusha.
	unsigned int InterruptVector, ErrorCode;					//Interrupt number and error code (if applicable)
	unsigned int EIP, CS, EFLAGS, USERESP, SS;					//Pushed by the processor automatically.
};

struct InitKernelMem
{
    UINT32 dwKernelSize;
    void * lpKernelWorkingMemory;

    void * lpGDT;
    void * lpIDT;
    void * lpGDTBitmap;

    void * lpKernelPageBitmap;

    void * lpKernelPageDirectory;
    void * lpKernelPageTables;

    UINT32 dwPhysicalTotal;
    UINT32 dwPhysicalAvailable;

    void * lpPhysicalBitmap;
    UINT16 wPhysicalBitmapSize;
};

#ifndef _HHEAPINFO_DEFINED
    #define _HHEAPINFO_DEFINED
    struct Heap
    {
        UINT32 dwSize;             //size of the memory segement
        BOOLEAN IsUsed;          //Free or Occupied
        struct Heap * Next;      //pointer to next memory segment
        struct Heap * Previous;  //pointer to next memory segment
    }__attribute__ ((packed));
    typedef struct Heap HEAP;
    typedef HEAP * LPHEAP;

    struct HeapInfo
    {
        LPHEAP hHeapRoot;
        UINT32 dwOptions;
        UINT32 dwMaximumSize;
        UINT32 dwCurrentSize;
    }__attribute__ ((packed));
    typedef struct HeapInfo HEAPINFO;
    typedef HEAPINFO * LPHEAPINFO;
#endif

#endif /* COMMONSTRUCT_H_ */

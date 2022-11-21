/*
 * MemoryMgr.cpp
 *
 *  Created on: 2012. 2. 26.
 *      Author: darkx
 */

#include "MemoryMgr.h"
#include "DescriptorTables.h"
#include "BitmapFunc.h"
#include "../03.Library/CONSOLE.h"
#include "../03.Library/Util.h"
#include "../03.Library/printf.h"
#include "../03.Library/Error.h"

#include "IDT.h"
#include "PageFunc.h"
#include "../05.Task/TaskManager.h"
#include "../05.Task/Task.h"

extern Console OrangeConsole;



//This macro is used for setting kernel page directory entries (PDE) and Kernel's Page Bitmap
//note :- this will not set the Physical Page Bitmap
#define INIT_MAP_PAGES(kp_START, kp_TOTAL, kp_ADDRESS, Protection ) \
            for(i=(kp_START),lpHAddress=(UINT32 *)(kp_ADDRESS) ;i<(kp_START)+(kp_TOTAL);i++,lpHAddress= (void*)((UINT32)lpHAddress + VASM_PAGE_SIZE  )) \
                INIT_SetPageEntry(i, lpHAddress, Protection ); \
            SetBitsInBitmap((BYTE *)lpInitKMem->lpKernelPageBitmap, PAGE_USED, kp_START , kp_TOTAL) ;

void * mm_GDT=NULL;
void * mm_IDT=NULL;                             //Interrupt Vector Table of Kernel
void * mm_GDTBitmap=0;                          //Global Descriptor Table's Bitmap, initialized during Phase I of InitMem

void * mm_KernelPageBitmap;                   //Kernel's Page Bitmap intalized during Phase I of InitMem

void * mm_KernelPageDirectory;                  //Kernel Page Directory intalized during Phase I of InitMem
//void * mm_KernelTPT;                          //Kernel Translation Page Table intalized during Phase I of InitMem
//void * mm_KernelPointerToTPT;                 //Kernel Translation Pointer for Translation Page Table intalized during Phase I of InitMem

/*Physical memory management variables*/
extern void * sbss, * g_code;
extern UINT32 mm_PhysicalTotal;
extern UINT32 mm_PhysicalAvailable;
extern UINT16 mm_PhysicalBitmapSize;
extern void * mm_PhysicalBitmap;

using namespace Orange;

void SetUpMemory()
{
	//Task* pSystemTask = TaskManager::GetSystemTask();

    struct InitKernelMem * lpInitKMem=(struct InitKernelMem *)KERNEL_TMP_VARIABLE_HOLD;

    int i;
    UINT32 dwKernelSize;
    void * lpKernelWorkingMemory;

    dwKernelSize = lpInitKMem->dwKernelSize;
    lpKernelWorkingMemory = lpInitKMem->lpKernelWorkingMemory;

    DescriptorPointer* pSysGDT = (DescriptorPointer*)KERNEL_TMP_DESCRIPTORPOINTER;

    pSysGDT->Address = (UINT32)lpInitKMem->lpGDT - (UINT32)lpInitKMem->lpKernelWorkingMemory + kp_KERNEL_MEM_LINEAR_ADDRESS;
    pSysGDT->Limit = (INITIAL_GDT_ENTRIES*8)-1 ;


    asm volatile ("lgdt %0" : :"m"(*pSysGDT));

    DescriptorPointer* pSysIDT = (DescriptorPointer*)(KERNEL_TMP_DESCRIPTORPOINTER + sizeof(DescriptorPointer));

    //load IDT value into system IDT register
    pSysIDT->Address = (UINT32)lpInitKMem->lpIDT - (UINT32)lpInitKMem->lpKernelWorkingMemory + kp_KERNEL_MEM_LINEAR_ADDRESS;
    pSysIDT->Limit = (256*8) - 1;                //set all interrupt / falut controller - review code
    asm volatile ("lidt %0" : :"m"(*pSysIDT));

    //mm_DataSegment_Init();


    mm_GDTBitmap = (void *)( (UINT32)lpInitKMem->lpGDTBitmap - (UINT32)lpInitKMem->lpKernelWorkingMemory + kp_KERNEL_MEM_LINEAR_ADDRESS);

    mm_GDT = (void *)( (UINT32)lpInitKMem->lpGDT - (UINT32)lpInitKMem->lpKernelWorkingMemory + kp_KERNEL_MEM_LINEAR_ADDRESS);
    mm_IDT = (void *)( (UINT32)lpInitKMem->lpIDT - (UINT32)lpInitKMem->lpKernelWorkingMemory + kp_KERNEL_MEM_LINEAR_ADDRESS);

    mm_PhysicalBitmap=(void *)((UINT32)lpInitKMem->lpPhysicalBitmap - (UINT32)lpInitKMem->lpKernelWorkingMemory + kp_KERNEL_MEM_LINEAR_ADDRESS);

    mm_KernelPageBitmap = ((void *)((UINT32)lpInitKMem->lpKernelPageBitmap - (UINT32)lpInitKMem->lpKernelWorkingMemory + kp_KERNEL_MEM_LINEAR_ADDRESS));
    mm_KernelPageDirectory = ((void *)((UINT32)lpInitKMem->lpKernelPageDirectory - (UINT32)lpInitKMem->lpKernelWorkingMemory + kp_KERNEL_MEM_LINEAR_ADDRESS));


    //these are normal variable taking the value
    mm_PhysicalTotal=lpInitKMem->dwPhysicalTotal;
    mm_PhysicalAvailable=lpInitKMem->dwPhysicalAvailable;
    mm_PhysicalBitmapSize=lpInitKMem->wPhysicalBitmapSize;


    /*copying the PDE values into Translation Page Table
    so that we can access the Page Tables without Page Fault
    */
    for(i=0;i<1024;i++)
    {
        UINT32 PDEValue=GetPageDirectoryEntry(i, mm_KernelPageDirectory );
        ((UINT32 *)VASM_LPTPT)[i] = PDEValue;
    }

    //initialize memory variables used by Physical Page Structures
    VASM_InitLowPhysicalPageStructures();

    IDT::SetupIDT();
}

/* This function Initializes the Memory Manager.
This is phase I of the Memory Initialization, this will setup the necessary PDE and PTE
Note :-
    Corresponding Page Tables should intialized before intializing Pages.

After calling this function the Kernel sets the Page Bit in CR0.
*/



void* InitKernelWorkingMemory(multiboot_header_t* pBoot,  multibootInfo *mbootPtr)
{
	printf("Orange OS Start\n");

	printf("Start Address : 0x%x\n", pBoot->entry_addr);

	UINT32 dwTotalPhysicalMemory = 0;

	struct InitKernelMem * lpInitKMem = NULL;

	UINT32 i = 0;
	void * lpWorkingMemoryTop = NULL;                  //this memory contains next free memory during intialization
	void * lpHAddress = NULL;                          //this variable is used only in the macro and carries only temporary values
	UINT32 dwKernelSize = 0;
	void * lpKernelWorkingMemory = NULL;               //Should not modify the content of this variable
	struct GDTEntry InitGDTValues[INITIAL_GDT_ENTRIES]=
	{
	     {0, 0, 0, 0, 0, 0},                       //null
	     {0xFFFF, 0, 0, 0x9A, 0xDF, 0},        //kernel code (PRESENT | CSEG | READ)
	     {0xFFFF, 0, 0, 0X92, 0xDF, 0},        //kernel data
	     {0xFFFF, 0, 0, 0xBA, 0xDF, 0},
	      {0xFFFF, 0, 0, 0xB2, 0xDF, 0}
	};

	lpInitKMem=(struct InitKernelMem *)KERNEL_TMP_VARIABLE_HOLD;                           //this address should be same after initializing paging

	dwTotalPhysicalMemory = 1 + mbootPtr->memoryHigh/1024;                           //Storing Amount of Physical Memory in MB
	dwKernelSize = pBoot->bss_end_addr - pBoot->load_addr ;                                                           //Size of Kernel Code, Data, BSS sections
	lpInitKMem->dwKernelSize = dwKernelSize;

	lpInitKMem->dwPhysicalAvailable = lpInitKMem->dwPhysicalTotal = dwTotalPhysicalMemory; //Total memory availale to the system

	printf("Kernel Size : 0x%x\n", lpInitKMem->dwKernelSize);
	printf("Total Physical Memory : %d MB\n", lpInitKMem->dwPhysicalTotal);


	lpKernelWorkingMemory = (void *) (KERNEL_BASE_ADDESS +  ( dwKernelSize / VASM_PAGE_SIZE ) * VASM_PAGE_SIZE \
			+ ( ( dwKernelSize % VASM_PAGE_SIZE ) ? VASM_PAGE_SIZE : 0 ) \
			+ ( kp_KERNEL_STACK_TOTAL_PAGES * VASM_PAGE_SIZE ));

	lpInitKMem->lpKernelWorkingMemory = lpKernelWorkingMemory;


	    lpInitKMem->lpPhysicalBitmap =  lpKernelWorkingMemory;                              //Physical Bitmap Start Address
	    lpInitKMem->wPhysicalBitmapSize = ( dwTotalPhysicalMemory / 128 )+1;                //pages required for managing physical free bitmap
	        //setting all the bits in the Physical Bitmap to 0
	        for(i=0;i<(UINT32)lpInitKMem->wPhysicalBitmapSize*VASM_PAGE_SIZE/4;i++)
	            ((UINT32 *)lpInitKMem->lpPhysicalBitmap)[i]=0;

	//kernel's virtual space usage bitmap
	 lpInitKMem->lpKernelPageBitmap = (void*)((UINT32)lpInitKMem->lpPhysicalBitmap + (lpInitKMem->wPhysicalBitmapSize * VASM_PAGE_SIZE)) ;

	 printf("Physical Memory Page Bitmap Address : 0x%x, wPhysicalBitmapSize : 0x%x\n", lpInitKMem->lpPhysicalBitmap,  lpInitKMem->wPhysicalBitmapSize);
	 printf("Kernel Memory Page Bitmap Address  : 0x%x\n", lpInitKMem->lpKernelPageBitmap);
/////////////////////////////////////////////////////////////////////////////////////////////////
//커널의 가상 페이지 관련 세팅 4GB 바이트에 대한 페이지의 수는 4GB / 4K = 131072 이다.
//32비트 자료형인 unsigned int는 32개의 페이지에 대해 기술할 수 있다. 따라서 커널의 가상 페이지를 모두 표시하기 위해
//32768개의 unsigned int가 필요하며 131072바이트가 필요하다.
//아래 루프에서는 한번에 4바이트씩 처리하므로 32768번을 돌면 4GB 전체에 대한 가상 페이지를 만들 수 있고
//윈도우즈에서 커널은 상위 2기가에서 존재하는 것처럼 보이게 하는 처럼 2기가씩 분리하여 세팅을 하고 있다.
//상위 0-2기가 사이에서 커널이 접근하는 일은 없을 것이며
//2-4기가 바이트 영역은 프로세스가 공유할 수 있도록 하게 할 것이다.
/////////////////////////////////////////////////////////////////////////////////////////////////
	 for(i=0;i<(VASM_PAGE_BITMAP_SIZE*VASM_PAGE_SIZE/(2*4) );i++)                        //0-2GB
		 ((UINT32 *)lpInitKMem->lpKernelPageBitmap)[i]=0xFFFFFFFF;                       //Mark as Reserved

	 for(;i<(VASM_PAGE_BITMAP_SIZE*VASM_PAGE_SIZE/4);i++)                                //2-4 GB
		 ((UINT32 *)lpInitKMem->lpKernelPageBitmap)[i]=0;                                //Mark as Free


	 //Creating Page Directory Address & intial page tables
	     lpInitKMem->lpKernelPageDirectory = (void*)((UINT32)lpInitKMem->lpKernelPageBitmap + (VASM_PAGE_BITMAP_SIZE * VASM_PAGE_SIZE));
	     lpInitKMem->lpKernelPageTables = (void*)((UINT32)lpInitKMem->lpKernelPageDirectory + VASM_PAGE_SIZE);
	     //setting Page Directory Entries
	     for( i=0; i<(VASM_PAGE_SIZE/4); i++)                                                                 //setting all page table as invalid
	         ((UINT32 *) lpInitKMem->lpKernelPageDirectory )[i] = 0;

	 printf("Kernel Page Directory Address : 0x%x\n", lpInitKMem->lpKernelPageDirectory);

	     //create GDT
	     lpWorkingMemoryTop = (void*)((UINT32)lpInitKMem->lpKernelPageTables + VASM_PAGE_SIZE);
	     lpInitKMem->lpGDT = lpWorkingMemoryTop;
	     //Initialize GDT with NULL
	     for( i=0; i<(GDT_TOTAL_PAGES * VASM_PAGE_SIZE)/4; i++ )
	         ((UINT32 *) lpInitKMem->lpGDT )[i] = 0;
	     //copying the initial GDT entries

	     for(i=0;i<  (sizeof(struct DescriptorPointer)/2 )*INITIAL_GDT_ENTRIES;i++)
	         ((UINT32 *) lpInitKMem->lpGDT )[i] = ((UINT32 *)InitGDTValues)[i];

	     //create IDT
	         lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + (GDT_TOTAL_PAGES * VASM_PAGE_SIZE));
	         lpInitKMem->lpIDT = lpWorkingMemoryTop;
	         //Initialize IDT with NULL
	         for( i=0; i<(IDT_TOTAL_PAGES * VASM_PAGE_SIZE)/4; i++ )
	             ((UINT32 *) lpInitKMem->lpIDT )[i] = 0;


	         lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + IDT_TOTAL_PAGES * VASM_PAGE_SIZE);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	         //create GDT and IDT Bitmap
	             lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + VASM_PAGE_SIZE);
	             lpInitKMem->lpGDTBitmap = lpWorkingMemoryTop;
	             //setting all GDT entries as free
	             for(i=1;i<VASM_PAGE_SIZE/4;i++)
	                 ((UINT32 *)lpInitKMem->lpGDTBitmap)[i]=0;                                       //All are free
	             ((BYTE *)lpInitKMem->lpGDTBitmap)[0]=0x3F;                                          //First six entries are occupied

	             ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	             //아마도 ACE OS의 저자는 IDT의 경우는 동적으로 할당하고 세팅할 경우가 없으니까 패스한 것 같다.
	             ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	             //initializing Page Tables
	             //Oth Page Table for 1st Reserved 1MB
	             lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + VASM_PAGE_SIZE);
	             INIT_SetPageDirectoryEntry(0, lpInitKMem->lpKernelPageDirectory , lpWorkingMemoryTop , 3);


	             lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + VASM_PAGE_SIZE);

//페이지 디렉토리 테이블은 하나당 1024개의 PDE를 담을 수 있다. 따라서 1024로 나눠준다.
	             for(i=kp_KERNEL_START_PAGE/1024;i<=(kp_KERNEL_START_PAGE+kp_KERNEL_TOTAL_PAGES)/1024 ;
	            i++,lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + VASM_PAGE_SIZE))  //setting page tables for kernel code
	                 INIT_SetPageDirectoryEntry(i, lpInitKMem->lpKernelPageDirectory , (void*)((UINT32)lpWorkingMemoryTop), 3);
	             //for(i=kp_KERNEL_MEM_START/1024;i<=(kp_KERNEL_MEM_START+kp_KERNEL_MEM_TOTAL_PAGES)/1024 ;i++,lpWorkingMemoryTop+=VASM_PAGE_SIZE)  //setting page tables for kernel working memory
	             //    INIT_SetPageDirectoryEntry(i, lpInitKMem->lpKernelPageDirectory , lpWorkingMemoryTop + ( (i+1)*VASM_PAGE_SIZE ), 3);

	             lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + VASM_PAGE_SIZE);
	             //creating PointerToTPT & Translation Page Table PageTable and pointing them in PDE

	             INIT_SetPageDirectoryEntry( kp_POINTER_TPT_START/1024, lpInitKMem->lpKernelPageDirectory, lpWorkingMemoryTop , 3 );
	                 SetBitsInBitmap((BYTE *)lpInitKMem->lpPhysicalBitmap, PAGE_USED, (UINT32)lpWorkingMemoryTop/VASM_PAGE_SIZE , 1 ) ;
	              lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + VASM_PAGE_SIZE);


	                 INIT_SetPageDirectoryEntry(kp_TRANSLATION_PT_START/1024, lpInitKMem->lpKernelPageDirectory, lpWorkingMemoryTop , 3 );
	                 SetBitsInBitmap((BYTE *)lpInitKMem->lpPhysicalBitmap, PAGE_USED, (UINT32)lpWorkingMemoryTop/VASM_PAGE_SIZE , 1 ) ;

	                 SetBitsInBitmap((BYTE *)lpInitKMem->lpKernelPageBitmap, PAGE_USED, kp_TRANSLATION_PT_START , kp_TRANSLATION_PT_TOTAL) ;

	                 //pointing the Translation Page Table
	                 for(i=kp_POINTER_TPT_START;i<kp_POINTER_TPT_START+kp_POINTER_TPT_TOTAL;
	                		 i++,lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + VASM_PAGE_SIZE)  )
	                     INIT_SetPageEntry(i, lpWorkingMemoryTop, 3  );
	                 SetBitsInBitmap((BYTE *)lpInitKMem->lpKernelPageBitmap, PAGE_USED, kp_POINTER_TPT_START , kp_POINTER_TPT_TOTAL ) ;

	                 /* Hereafter lpWorkingMemoryTop should not be used, because accessing the memory after this will not be included in the paging table */

	                 // reserving first 1 MB in physical memory, so that no process can allocate it
	                 //mm_C_S_BitsInBitmap(lpInitKMem->lpPhysicalBitmap, PAGE_USED, 0 , (1024*1024)/4096 );
	              for(i=0;i<8;i++)
	                     ((UINT32 *)lpInitKMem->lpPhysicalBitmap)[i]=0xFFFFFFFF;

	                 //Intiliazing Kernel Pages
	                 INIT_MAP_PAGES(  kp_RESERVED_START_PAGE, kp_RESERVED_TOTAL_PAGES, kp_RESERVED_ADDRESS, 3);
	                 SetBitsInBitmap((BYTE *)lpInitKMem->lpPhysicalBitmap, PAGE_USED, kp_RESERVED_START_PAGE , kp_RESERVED_TOTAL_PAGES ) ;

	                 INIT_MAP_PAGES(  kp_KERNEL_STACK_START_PAGE, kp_KERNEL_STACK_TOTAL_PAGES, kp_KERNEL_STACK_ADDRESS, 3 );
	                 SetBitsInBitmap((BYTE *)lpInitKMem->lpPhysicalBitmap, PAGE_USED, kp_KERNEL_STACK_START_PAGE, kp_KERNEL_STACK_TOTAL_PAGES ) ;




	                 INIT_MAP_PAGES(  kp_KERNEL_START_PAGE, kp_KERNEL_TOTAL_PAGES, kp_KERNEL_ADDRESS, 3 );
	                 SetBitsInBitmap((BYTE *)lpInitKMem->lpPhysicalBitmap, PAGE_USED, kp_KERNEL_ADDRESS/VASM_PAGE_SIZE , kp_KERNEL_TOTAL_PAGES ) ;

	                 lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop + VASM_PAGE_SIZE);
	                 int iTotalPages=(kp_KERNEL_MEM_START+kp_KERNEL_MEM_TOTAL_PAGES)/1024;

	                 for(i=(kp_KERNEL_MEM_START/1024)+1;i<=(UINT32)iTotalPages;i++,
	                 	 lpWorkingMemoryTop = (void*)((UINT32)lpWorkingMemoryTop+ VASM_PAGE_SIZE))  //setting page tables for kernel working memory
	                    INIT_SetPageDirectoryEntry(i, lpInitKMem->lpKernelPageDirectory , (void*)((UINT32)lpWorkingMemoryTop), 3);

	                 INIT_MAP_PAGES(  kp_KERNEL_MEM_START, kp_KERNEL_MEM_TOTAL_PAGES, kp_KERNEL_MEM_ADDRESS, 3 );
	                 SetBitsInBitmap((BYTE *)lpInitKMem->lpPhysicalBitmap, PAGE_USED, kp_KERNEL_MEM_ADDRESS/VASM_PAGE_SIZE, kp_KERNEL_MEM_TOTAL_PAGES ) ;

	                 //remember these entries should be removed after successful intialization of PAGING
	                 INIT_MAP_PAGES(  kp_K_TMP_MAP_StartPage, kp_K_TMP_MAP_TotalPages, kp_K_TMP_MAP_Address, 3  );
	                 SetBitsInBitmap((BYTE *)lpInitKMem->lpPhysicalBitmap, PAGE_USED, kp_K_TMP_MAP_StartPage , kp_K_TMP_MAP_TotalPages ) ;

	                 asm volatile("movl %0, %%ebx" : :"r" ((kp_KERNEL_STACK_START_PAGE + kp_KERNEL_STACK_TOTAL_PAGES) * VASM_PAGE_SIZE) ); //copy new stack base value - it will be copied into esp

	                 printf("Stack Top Address : %x\n", ((kp_KERNEL_STACK_START_PAGE + kp_KERNEL_STACK_TOTAL_PAGES) * VASM_PAGE_SIZE));

	   return lpInitKMem->lpKernelPageDirectory;                                             //copy page directory address into eax






	//return true;
}

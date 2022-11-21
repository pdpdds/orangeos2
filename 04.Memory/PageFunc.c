/*
 * PageFunc.cpp
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#include "PageFunc.h"
#include "../03.Library/CommonStruct.h"
#include "../03.Library/Error.h"
#include "DescriptorTables.h"
#include "BitmapFunc.h"

#define KERNEL_TMP_VARIABLE_HOLD        (0x19000)               //here the the InitKernelMem structure is placed temporarily
struct DescriptorPointer sysGDT __attribute__ ((aligned (32))) ;

extern void* mm_GDTBitmap;
extern void * mm_GDT;

namespace Orange {

UINT16 CreateGDTEntry(void * BaseAddress, UINT32 Limit, BYTE G_DB, BYTE Type_DPL)
{
    UINT32 FirstDWord, SecondDWord, tmpValue;
    UINT16 FreeGDTNo;

    //finding free sysGDT entry
    FreeGDTNo = mm_FindInBitmap((BYTE*) mm_GDTBitmap, PAGE_FREE, 0, 1, (GDT_TOTAL_PAGES-1) * VASM_PAGE_SIZE );
    if ( FreeGDTNo == 0 )
    {
        //printf("\n CreateGDTEntry Failed :: No free sysGDT in Bitmap %X", mm_GDTBitmap);
        return 0;
    }
    asm volatile ("sgdt %0" : :"m"(sysGDT));
    //dbgprintf("\n Base Address %X Limit %X G_DB %X Type_DPL %X", BaseAddress, Limit, G_DB, Type_DPL );
    FirstDWord = (UINT32)BaseAddress;
    FirstDWord<<=16;
    tmpValue=Limit & 0x0000FFFF;
    FirstDWord|=tmpValue;

    SecondDWord = (UINT32)BaseAddress;          //setting bits 0-7
    SecondDWord>>=16;
    SecondDWord&=0x000000FF;            //filtering 8-15

    tmpValue=(UINT32)BaseAddress;               //setting 24-31
    tmpValue>>=24;                      //removing 0-23
    tmpValue<<=24;                      //moving to 23 position
    SecondDWord|=tmpValue;

    tmpValue=Limit;
    tmpValue>>=16;
    tmpValue|=Type_DPL;
    tmpValue<<=8;
    SecondDWord|=tmpValue;

    tmpValue=G_DB;                    //setting 16-23
    tmpValue<<=16;
    SecondDWord|=tmpValue;


    //BYTE * lpGDT=(BYTE *)KernelMap((void *)0, VASM_PAGE_SIZE*2,(UINT32 *) (((UINT32)sysGDT.dwBase/VASM_PAGE_SIZE)*VASM_PAGE_SIZE) );
    //UINT32 PageGap=(UINT32)sysGDT.dwBase - ((UINT32)sysGDT.dwBase/VASM_PAGE_SIZE)*VASM_PAGE_SIZE;
    //UINT32 * lpGDTEntry=(UINT32 *)((UINT32)lpGDT+PageGap+((FreeGDTNo-1)*8));

    UINT32 dwPageGap = (UINT32)sysGDT.Address - ( (UINT32)sysGDT.Address/VASM_PAGE_SIZE ) * VASM_PAGE_SIZE;
    UINT32 * lpGDTEntry = (UINT32 *)( (UINT32)mm_GDT + dwPageGap + ( (FreeGDTNo-1)*8) );

    lpGDTEntry[0]=FirstDWord;
    lpGDTEntry[1]=SecondDWord;

    //increase sysGDT limit
    sysGDT.Limit = (FreeGDTNo*8)-1 ;
    //load new sysGDT
    asm volatile ("lgdt %0" : :"m"(sysGDT));

    //setting the GDTEntry as used
    SetBitsInBitmap((BYTE *)mm_GDTBitmap, PAGE_USED, FreeGDTNo, 1) ;

    //dbgprintf("\n First %X %X", FirstDWord, SecondDWord);
    return (FreeGDTNo-1)*8;//return sysGDT Selector
}



/*
Call this function during Phase I of the InitMem to set the PDE
Input :
    PDENo               -       Page Directory Entry No to set
    lpPageDirectory     -       Page Directory Physical Address
    lpPTAddress         -       Physical address of Page Table
    Protection          -       Protection ( 3 - for Supervisor and Present )
*/
void INIT_SetPageDirectoryEntry(int PDENo, void * lpPageDirectory, void * lpPTAddress, BYTE Protection )
{
    if ( PDENo >= 1024 || PDENo < 0 )
        return;
    UINT32 PDEValue=(UINT32)lpPTAddress;         //PageTable Address
    PDEValue=(PDEValue & 0x0FFFFF000) | Protection;

    ((UINT32 *)lpPageDirectory)[PDENo] = PDEValue; //Page Directory Entry is 4 byte long
}

/*The following function sets the corresponding Page Directory Entry in the Page Directory
Call this function during Phase II of the InitMem to set the PDE
Input :
    PDENo               -       Page Directory Entry No to set
    lpPageDirectory     -       Page Directory linear Address
    lpPageTableList     -       Pointer to TPT
    Protection          -       Protection ( 3 - for Supervisor and Present )
*/
void SetPageDirectoryEntry(int PDENo, void * lpPageDirectory, void * lpPageTableList, void * lpPTAddress, BYTE Protection )
{
    if ( PDENo >= 1024 || PDENo < 0 )
        return;
    UINT32 PDEValue=(UINT32)lpPTAddress;         //PageTable Address
    PDEValue=(PDEValue & 0x0FFFFF000) | Protection;
    #ifdef _DEBUG_
        dbgprintf("\n\t VASM :: SetPageDirectoryEntry(%d) lpPageTableList %X lpPageDirectory %X, PDEValue %X ", PDENo, lpPageTableList, lpPageDirectory, PDEValue);
    #endif
    ((UINT32 *)lpPageTableList)[PDENo] = PDEValue;
    ((UINT32 *)lpPageDirectory)[PDENo] = PDEValue; //Page Directory Entry is 4 byte long
}

int INIT_SetPageEntry(UINT32 PageNo, void * lpPhysicalAddress, BYTE Protection)
{
    int PTNo=PageNo/1024;               //Page Directiry Entry No
    UINT32 PageTable, PTEValue;
    struct InitKernelMem * lpInitKMem=(struct InitKernelMem *)KERNEL_TMP_VARIABLE_HOLD;

    PageNo = PageNo%1024;               //Actual Page No
    PageTable=((UINT32 *)lpInitKMem->lpKernelPageDirectory)[PTNo];
    if ( !(PageTable & 1) )
    {
       // DEBUG();
    	int i = 5;
    	int j = 0;

    	i = i / j;
        return PTNo; //Page Table not present
    }

    PageTable&=0xFFFFF000;  //extract the page table address
    PTEValue=(UINT32)lpPhysicalAddress;                 //storing Physical Address
    PTEValue=(PTEValue & 0x0FFFFF000)|Protection;       //zeros all except the upper 20 bits and sets Supervisor and Present bits
    ((UINT32 *)PageTable)[PageNo] = PTEValue;           //Page Table Entry is 4 byte long

    return 0; //success
}
/* This function sets the specified page entry in a page table.
Donot call this function before intializing Paging.
Note:-****
 1) The given Page Directory is used only for checking whether the specified Page Table is
    present in the memory or not. But there may be a design change which may eliminate this
    parameter(lpPageDirectory).
 2) You must fill the address of the empty Page Table in the Page Directory before calling this function
 3) It will not set the page bitmap and physical free bitmap
Input :
    PageNo                      - Page No ( 0 - 1024*1024 )
    lpPageDirectory             - Address of the Page Directory ( pointer in the kernel's virtual address space)
    TranslationPageTable        - Translation Page Table
    lpPhysicalAddress           - Physical address of Page Table to set in PD
    Protection                  - Protection and Present value
Returns :
    0 - Success
    1 - The Page Table Entry is invalid or not present
*/
int SetPageEntry(UINT32 dwPageNo, void * lpPageDirectory, UINT32 * TranslationPageTable, void * lpPhysicalAddress, BYTE Protection)
{
    int PageNo = dwPageNo;
    int PTNo=PageNo/1024;       //Page Directiry Entry No
    UINT32 PageTable, PTEValue;
    #ifdef _DEBUG_
        dbgprintf("\n\t VASM :: SETPAGEENTRY() PageNo %d lpPageDirectory %X lpPhysicalAddress %X",PageNo, lpPageDirectory,lpPhysicalAddress  );
    #endif

    PageNo = PageNo%1024;                //Actual Page No
    PageTable=((UINT32 *)lpPageDirectory)[PTNo];
    if ( !(PageTable & 1) )
    {
        #ifdef _DEBUG_
            dbgprintf("\n VASM :: SETPAGEENTRY() - Page Table Not Present %d", PTNo );
        #endif
        return 1; //Page Table not present
    }
    UINT32 * lpMappedPT = (UINT32 *)( (UINT32)TranslationPageTable + (PTNo*VASM_PAGE_SIZE )  ) ;
    #ifdef _DEBUG_
        dbgprintf("\n VASM :: SETPAGEENTRY() PTList at %X", lpMappedPT );
    #endif

    PTEValue=(UINT32)lpPhysicalAddress;          //storing physical Address
    PTEValue=(PTEValue & 0x0FFFFF000)|Protection;       //zeros all except the upper 20 bits and sets Supervisor and Present bits

    #ifdef _DEBUG_
        dbgprintf("\n VASM :: SETPAGEENTRY() Creating Entry in Page Table %X at %d value %X", lpMappedPT , PageNo, PTEValue);
    #endif
    ((UINT32 *)lpMappedPT)[PageNo] = PTEValue;           //Page Table Entry is 4 byte long
    __asm__ __volatile__ ("invlpg (%0)": :"r" ( PageNo * VASM_PAGE_SIZE ) );

    return 0; //success
}
/*
this function extracts the PageEntry from the TPT
*/
UINT32 GetPageEntry(UINT32 PageNo, UINT32 * TranslationPageTable)
{
    int PTNo=PageNo/1024;       //Page Directiry Entry No
    #ifdef _DEBUG_
        dbgprintf("\n\t VASM :: GETPAGEENTRY() PageNo %d ",PageNo  );
    #endif

    PageNo = PageNo%1024;                //Actual Page No

    UINT32 * lpMappedPT = (UINT32 *)( (UINT32)TranslationPageTable + (PTNo*VASM_PAGE_SIZE )  ) ;
    #ifdef _DEBUG_
        dbgprintf("\n VASM :: SETPAGEENTRY() PTList at %X", lpMappedPT );
    #endif
    return ((UINT32 *)lpMappedPT)[PageNo];
}

/*Returns the content of the given page directory entry*/
UINT32 GetPageDirectoryEntry(int PDENo, void * lpPageDirectory)
{
    return ((UINT32 *)lpPageDirectory)[PDENo];
}

/* This function returns the size of a virtual page*/
UINT32 VASM_GetPageSize()
{
    return VASM_PAGE_SIZE;
}

UINT32 mm_FreePhysicalPages;
UINT32 mm_AvailablePhysicalPages;

#define TOTAL_LOW_PHYSICAL_PAGES ((1024*1024)/VASM_PAGE_SIZE)
#define LOW_PHYSICAL_PAGE_BITMAP_SIZE (TOTAL_LOW_PHYSICAL_PAGES/8)
BYTE mm_LowPhysicalPageBitmap[LOW_PHYSICAL_PAGE_BITMAP_SIZE];    //to manage the first 1 MB physical memory

UINT32 VASM_GetTotalPhysicalPages()
{
    return mm_FreePhysicalPages;
}
UINT32 VASM_GetAvailablePhysicalPages()
{
    return mm_AvailablePhysicalPages;
}
/*The following LowPhysicalPage functions will work only on the memory below 1MB
----It does not touch the orginal physical bitmap, instead it uses it own bitmap mm_LowPhysicalPageBitmap for tracking
*/
void VASM_InitLowPhysicalPageStructures()
{
    int i;
    //-------replace this code by asking BIOS for the memory map
    for(i=0;i<25;i++)
        mm_LowPhysicalPageBitmap[i]=0xFF;  // first 100 kb is occupied
    for(;i<LOW_PHYSICAL_PAGE_BITMAP_SIZE;i++)
        mm_LowPhysicalPageBitmap[i]=0x00;   //all are free
    for(i=160;i<256;i++)
        mm_LowPhysicalPageBitmap[i]=0xFF;  // first 640KB to 1MB is occupied
    //------------------------------------------
}

/* This function try to allocate the given number of pages below 1 MB*/
UINT32 VASM_AllocateLowPhysicalPages(UINT32 dwStartPage, UINT32 dwLimitPage, UINT32 dwSize)
{
    DWORD dwAllocatedPage=0;
    //check paramater range
    if ( dwStartPage > TOTAL_LOW_PHYSICAL_PAGES || dwSize > TOTAL_LOW_PHYSICAL_PAGES )
    {
       // SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    //allocate page by searching bitmap
    dwAllocatedPage = mm_FindInBitmap( mm_LowPhysicalPageBitmap, PAGE_FREE, dwStartPage, dwSize, dwLimitPage);
    if ( dwAllocatedPage )
    {
        //now reserve the pages
        SetBitsInBitmap( mm_LowPhysicalPageBitmap, PAGE_USED, dwAllocatedPage, dwSize );
        return dwAllocatedPage * VASM_PAGE_SIZE; //allocation succeed
    }
   // SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return 0; //allocation failed
}
UINT32 VASM_GetFirstLowPhysicalPage(UINT32 dwStartPage, UINT32 dwLimitPage)
{
    if ( dwStartPage > TOTAL_LOW_PHYSICAL_PAGES || dwLimitPage > TOTAL_LOW_PHYSICAL_PAGES )
    {
       // SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return mm_FindInBitmap( mm_LowPhysicalPageBitmap, PAGE_FREE, dwStartPage, 1, dwLimitPage);
}
UINT32 VASM_FreeLowPhysicalPages(UINT32 dwStartPage, UINT32 dwSize)
{
    if ( dwStartPage > TOTAL_LOW_PHYSICAL_PAGES || dwSize > TOTAL_LOW_PHYSICAL_PAGES )
    {
      //  SetLastError(ERROR_INVALID_PARAMETER);
        return ERROR_INVALID_PARAMETER;
    }
    //clear the bitmap
    SetBitsInBitmap( mm_LowPhysicalPageBitmap, PAGE_FREE, dwStartPage, dwSize );
    return ERROR_SUCCESS;
}

} /* namespace Orange */

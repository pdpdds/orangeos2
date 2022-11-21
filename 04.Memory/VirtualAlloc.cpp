/*
 * VirtualAlloc.c
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#include "VirtualAlloc.h"
#include "DescriptorTables.h"
#include "BitmapFunc.h"
#include "../05.Task/TaskManager.h"
#include "../05.Task/Task.h"
#include "../03.Library/printf.h"
#include "PageFunc.h"

extern Orange::TaskManager TaskMgr;

extern void * mm_PhysicalBitmap;

namespace Orange {

/*UINT32 VirtualAlloc(UINT32 StartingAddress, UINT32 SizeRequired ,  UINT16 AllocationType, UINT16 ProtectionType)
{
    return VirtualAllocEx( GetTaskID(), StartingAddress, SizeRequired,  AllocationType, ProtectionType);
	return 0;
}*/
/*
This function allocates a range of linear address space in the given Task's Address Space
*/
UINT32 VirtualAllocEx(UINT32 TaskID, UINT32 StartingAddress, UINT32 SizeRequired ,  UINT16 AllocationType, UINT16 ProtectionType)
{

    printf("\n\r VASM :: VirtualAlloc() Size %d ",SizeRequired );

    UINT32 lpVirtualPage=0,j;
    UINT32 lpPhysicalPage;

    Task* pTask = TaskMgr.GetTask( TaskID );
    if ( pTask == NULL )
    {
    	printf("\n VASM :: Task(%d) Not Found ",TaskID);

        return 0;
    }
    //find the free page's starting address
    printf("\n VASM :: Search Bitmap %X, %X",pTask->GetPageBitmap(), VASM_PAGE_BITMAP_SIZE * VASM_PAGE_SIZE * 8 );
    if ( ! (AllocationType & (MEM_COMMIT | MEM_RESERVE )  ) ) //return if allocation type does not RESERVE or COMMIT
    {
    	printf("\n VASM :: Invalid Allocation type - %X", AllocationType );
        return 0;
    }
    if ( AllocationType & MEM_RESERVE )
    {
        lpVirtualPage = mm_FindInBitmap((BYTE *)pTask->GetPageBitmap(), PAGE_FREE , StartingAddress, SizeRequired , VASM_PAGE_BITMAP_SIZE * VASM_PAGE_SIZE * 8);
        printf("\n VASM :: Virtual Free Page Found at %X in PD %X",lpVirtualPage, pTask->GetTaskInfo()->pPageDirectory );
    }
    else
        lpVirtualPage = StartingAddress / VASM_PAGE_SIZE;

    if ( lpVirtualPage ) //if free page found
    {
        //mark the free pages as allocated
        SetBitsInBitmap((BYTE *)pTask->GetPageBitmap(), PAGE_USED, lpVirtualPage, SizeRequired );

        if ( ! (AllocationType & MEM_COMMIT) ) // if the page is not commited return
        {
        	printf("\n VASM :: VirtualAlloc() Returns %X",lpVirtualPage*VASM_PAGE_SIZE );
            return lpVirtualPage*VASM_PAGE_SIZE;
        }

        for(j=0;j<SizeRequired ;j++) //fill the task's page directory with correct information
        {

            //find a free page for the current virtual page
            //since virtual page should contigous and physical pages need not to be contigious
            lpPhysicalPage = mm_FindInBitmap( (BYTE *)mm_PhysicalBitmap, PAGE_FREE, 0, 1,  VASM_PAGE_BITMAP_SIZE * VASM_PAGE_SIZE );
            printf("\n VASM :: Physical Free Page Found at %X in %X",lpPhysicalPage, mm_PhysicalBitmap );

            if ( lpPhysicalPage ) // if found
            {
                //mark the physical storage as allocated
                SetBitsInBitmap((BYTE *)mm_PhysicalBitmap, PAGE_USED, lpPhysicalPage, 1 );

                printf("\n VASM :: Setting Page Entry - %X in PD %X",lpVirtualPage+j,pTask->GetTaskInfo()->pPageDirectory);

                if ( SetPageEntry(lpVirtualPage+j, pTask->GetPageDirectory(), (UINT32 *)VASM_TPT, (void *) (lpPhysicalPage * VASM_PAGE_SIZE), 3 ) ) //it will return 1 if page table is not fiiled
                {
                    //find a free page in physical free
                	printf("\n VASM :: Page Table not found. Creating new one");

                    UINT32 i;
                    UINT32 lpNewPageTable=mm_FindInBitmap((BYTE *)mm_PhysicalBitmap , PAGE_FREE , 0 , 1 , VASM_PAGE_BITMAP_SIZE * VASM_PAGE_SIZE) * VASM_PAGE_SIZE ;
                    if ( lpNewPageTable == 0 )
                    {

                    	printf("\n VASM :: No physical memory to create page table.");

                        return 0;
                    }

                    printf(" at physical address %X ", lpNewPageTable );
                        printf("\n VASM :: Setting Page Directory Entry %d, using lpTPT %X ", (lpVirtualPage+j)/1024, VASM_LPTPT );

                    SetPageDirectoryEntry( (lpVirtualPage+j)/1024 , pTask->GetPageDirectory(), VASM_LPTPT , (void *)lpNewPageTable , 3 );
                    SetBitsInBitmap((BYTE *)mm_PhysicalBitmap, PAGE_USED, (lpNewPageTable/VASM_PAGE_SIZE) , 1 );


                    printf("\n VASM :: Setup PageTable %d", ((lpVirtualPage+j)/1024));

                    //setup page table
                    for(i=0;i<VASM_PAGE_SIZE/4;i++)     //all entries are not present
                        ((UINT32 *)VASM_TPT)[(((lpVirtualPage+j)/1024)*4096)/4+i]=0;

                    printf("\n VASM :: Setting up Required Page " );

                    //setup the required page
                    if ( SetPageEntry(lpVirtualPage+j, pTask->GetPageDirectory(), (UINT32 *)VASM_TPT , (void*)(lpPhysicalPage * VASM_PAGE_SIZE) , 3) )
                    {

                    	printf("\n VASM :: Still unable to create Page Table " );

                        return 0;
                    }
                }
            }
            else
            {

            	printf("\n AllocHeap :: Commit Failed. No Free Physical Memory");

            }


            printf("\n VASM :: Marking Virutial Page Bitmap ");

            //mark the task's page bitmap as allocated
            //mm_C_S_BitsInBitmap(TI->m_PageBitmapAddress, PAGE_USED, lpVirtualPage , SizeRequired) ;
            if ( AllocationType & MEM_RESET )
            {
                //write code for zeroing the contents
            }
        }

        printf("\n VASM :: VirtualAlloc() Returns %X\n",lpVirtualPage*VASM_PAGE_SIZE );

        return lpVirtualPage*VASM_PAGE_SIZE; //success return the allocated memory
    }


    printf("\n VASM :: No Free Page is Found for Task(%d)",TaskID);


    return 0;
}

UINT32 VirtualFree(UINT32 StartingAddress, UINT32 Size, DWORD FreeType)
{
    //return VirtualFreeEx( GetTaskID(), StartingAddress, Size, FreeType );
	return 0;
}

/*********************This function is not completed
currently it just sets the corresponding bit in the Page Bitmap as free,
but it neither modifies the Physicals bitmap nor Page Directory entry
*/
UINT32 VirtualFreeEx(UINT32 TaskID, UINT32 StartingAddress, UINT32 Size, DWORD FreeType)
{
    UINT32 TotTasks=List_Count(TaskMgr.GetTaskList());
    UINT32 i;
    if ( FreeType & MEM_RELEASE )
    {
        if ( Size != 0 )
        {
            //SetLastError(ERROR_INVALID_PARAMETER);
            return 1;
        }
        for(i=0;i<TotTasks;i++)
        {
            Task * pTask = (Task*)List_GetData(TaskMgr.GetTaskList(),"",i);
            if( pTask->GetTaskID() == TaskID )
            {
                //Size/=VASM_PAGE_SIZE;        //divide the size in bytes by the Page size to get the required no of pages
                UINT32 UnReservedStart = mm_FindInBitmap((BYTE *)pTask->GetTaskInfo()->pPageBitmap, PAGE_USED, StartingAddress, 1, VASM_PAGE_BITMAP_SIZE * VASM_PAGE_SIZE * 8);
                Size = UnReservedStart - StartingAddress;
                if ( Size <= 0 )
                    return 2;
                SetBitsInBitmap((BYTE *)pTask->GetTaskInfo()->pPageBitmap, PAGE_FREE, StartingAddress, Size);

                //todo - write to code to unallocate the physical mapping
                return Size;
            }
        }
    }
    return 0;
}

} /* namespace Orange */

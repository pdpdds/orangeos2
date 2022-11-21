/*
 * PageFunc.h
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#ifndef PAGEFUNC_H_
#define PAGEFUNC_H_
#include "../03.Library/COMMONVAR.h"
#include "../03.Library/COMMON.h"

namespace Orange {

UINT16 CreateGDTEntry(void * BaseAddress, UINT32 Limit, BYTE G_DB, BYTE Type_DPL);

void INIT_SetPageDirectoryEntry(int PDENo, void * lpPageDirectory, void * lpPTAddress, BYTE Protection );
int INIT_SetPageEntry(UINT32 PageNo, void * lpPhysicalAddress, BYTE Protection);
int SetPageEntry(UINT32 dwPageNo, void * lpPageDirectory, UINT32 * TranslationPageTable, void * lpPhysicalAddress, BYTE Protection);
UINT32 GetPageEntry(UINT32 PageNo, UINT32 * TranslationPageTable);
UINT32 GetPageDirectoryEntry(int PDENo, void * lpPageDirectory);

void SetPageDirectoryEntry(int PDENo, void * lpPageDirectory, void * lpPageTableList, void * lpPTAddress, BYTE Protection );

UINT32 VASM_GetPageSize();

UINT32 VASM_GetTotalPhysicalPages();
UINT32 VASM_GetAvailablePhysicalPages();
void VASM_InitLowPhysicalPageStructures();
UINT32 VASM_AllocateLowPhysicalPages(UINT32 dwStartPage, UINT32 dwLimitPage, UINT32 dwSize);
UINT32 VASM_GetFirstLowPhysicalPage(UINT32 dwStartPage, UINT32 dwLimitPage);
UINT32 VASM_FreeLowPhysicalPages(UINT32 dwStartPage, UINT32 dwSize);
} /* namespace Orange */
#endif /* PAGEFUNC_H_ */

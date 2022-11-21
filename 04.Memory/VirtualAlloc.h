/*
 * VirtualAlloc.h
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#ifndef VIRTUALALLOC_H_
#define VIRTUALALLOC_H_
#include "../03.Library/COMMONVAR.h"
#include "../03.Library/COMMON.h"

namespace Orange {

//UINT32 VirtualAlloc(UINT32 StartingAddress, UINT32 SizeRequired ,  UINT16 AllocationType, UINT16 ProtectionType);
UINT32 VirtualAllocEx(UINT32 TaskID, UINT32 StartingAddress, UINT32 SizeRequired ,  UINT16 AllocationType, UINT16 ProtectionType);
UINT32 VirtualFree(UINT32 StartingAddress, UINT32 Size, DWORD FreeType);
UINT32 VirtualFreeEx(UINT32 TaskID, UINT32 StartingAddress, UINT32 Size, DWORD FreeType);

} /* namespace Orange */
#endif /* VIRTUALALLOC_H_ */

/*
 * BitmapFunc.h
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#ifndef BITMAPFUNC_H_
#define BITMAPFUNC_H_
#include "../03.Library/CommonVar.h"

namespace Orange {

extern "C" void SetBitsInBitmap(BYTE * Bitmap, BYTE ClearOrSet, UINT32 StartIndex, UINT32 Size);
extern "C" UINT32 FindContinuousBitsInBitmap(BYTE * lpBitmap, BYTE FindWhat, UINT32 StartIndex, UINT32 Size, UINT32 Limit);

UINT32 mm_FindInBitmap(BYTE * lpBitmap, BYTE FindWhat, UINT32 StartIndex, UINT32 Size, UINT32 Limit);
UINT32 mm_FindFirstInBitmap(UINT32 * Bitmap, BYTE FindWhat, UINT32 StartIndex, UINT32 Limit );

} /* namespace Orange */
#endif /* BITMAPFUNC_H_ */

/*
 * BitmapFunc.cpp
 *
 *  Created on: 2012. 2. 29.
 *      Author: darkx3
 */

#include "BitmapFunc.h"

namespace Orange {

/*This function searchs in a BYTE Bitmap from the StartIndex for the Bits of Size with set/cleared
Input:
    lpBitmap            -       Byte Bitmap
    FindWhat            -       0 - Free  1 - Used
    StartIndex          -       Index where to start searching
    Size                -       Size
    Limit               -       Limit
Returns:
    0           -       If no contious bit sequence of size is found
    NonZero     -       Index of the bit found
*/
UINT32 mm_FindInBitmap(BYTE * lpBitmap, BYTE FindWhat, UINT32 StartIndex, UINT32 Size, UINT32 Limit)
{
    UINT32 Result,Result2=0,Start=StartIndex/BITS_IN_UINT32;
    while( !Result2 )
    {
        //find the first free/used bit
        Result=mm_FindFirstInBitmap((UINT32 *)lpBitmap, FindWhat, Start , Limit/BITS_IN_UINT32 );
        //convert the uint32 index to bit index
        Start=Result+1;//still we have to search, so prepare the index
        Result*=BITS_IN_UINT32;
        //search for continous bits of specified size
        Result2 = FindContinuousBitsInBitmap(lpBitmap , FindWhat, Result , Size , Result+Size+32 );
        if ( Result2 ) //if a page is found return result
            return Result2;
    }
    return 0;
}
/*
This functions searches for a element in a 4byte unsigned int array
containing atleast one bit free

Input:
    lpBitmap            -       Byte Bitmap
    FindWhat            -       0 - Free  1 - Used
    StartIndex          -       Index where to start searching
    Limit               -       Limit
Returns:
    0           -       If no contious bit sequence of size is found
    NonZero     -       Index of the bit found
*/
UINT32 mm_FindFirstInBitmap(UINT32 * Bitmap, BYTE FindWhat, UINT32 StartIndex, UINT32 Limit )
{
    UINT32 i=StartIndex;
    if ( FindWhat == PAGE_FREE )
        while( Bitmap[i] == 0xFFFFFFFF && i<Limit )
            i++;
    else
        while( Bitmap[i] == 0 && i<Limit)
            i++;

    return (i<Limit)?i:0;       //return 0 if i>=limit
}

} /* namespace Orange */

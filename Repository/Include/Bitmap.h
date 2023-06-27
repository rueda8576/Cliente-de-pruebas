#ifndef AVT_BITMAP_H_
#define AVT_BITMAP_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define THREE_CHANNEL   0xC
#define BMP_HEADER_SIZE 54
#define ALIGNMENT_SIZE  4

typedef enum
{
    ColorCodeMono8  = 1,
    ColorCodeMono16 = 2,
    ColorCodeBGR24  = 4,
    ColorCodeRGB24  = 8
} ColorCode;

typedef struct
{
    void*           buffer;
    unsigned long   bufferSize;
    unsigned long   width; 
    unsigned long   height;
    ColorCode       colorCode;
} AVTBitmap;

/**
 * @brief Creates a MS Windows bitmap with header and color palette and fills it with the content of the given byte buffer
 * @param pBitmap A pointer to an AVTBitmap that will get filled
 * @param pBuffer The buffer that will be used to fill the created bitmap
 * @return 0 in case of error or 1 in case of success
*/
unsigned char AVTCreateBitmap( AVTBitmap * const pBitmap, const void* pBuffer );

/**
 * @brief Releases a given bitmap
 * @param pBitmap The bitmap whose memory will be freed
 * @return 0 in case of error or 1 in case of success
*/
unsigned char AVTReleaseBitmap( AVTBitmap * const pBitmap );

/**
 * @brief Writes a given bitmap to file
 * @param pBitmap The AVTBitmap to write to file
 * @param pFileName The destination (complete path) where to write the bitmap to
 * @return 0 in case of error or 1 in case of success
*/
unsigned char AVTWriteBitmapToFile( AVTBitmap const * const pBitmap, char const * const pFileName );

#endif

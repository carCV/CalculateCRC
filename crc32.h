#ifndef CRC32_H
#define CRC32_H
#include "type.h"


#define CRC32_POLY_LE 0xedb88320

union U32_BYTE {
    u32 uData;
    unsigned char cArray[4];
};

u32 crc32_le(u32 size, u8 const *data);

void GenerateCRC32Ex(
        const unsigned char*  iArray,			/* Array for the Data [in] */
        unsigned int          iSize,			/* Length of the array for the Data [in] */
        unsigned char*        ioArray,          /* Array for the Data [in, out] */
        unsigned int&         oSize             /* Length of the Data [out] */
        );

#endif // CRC32_H

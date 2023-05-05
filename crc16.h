#ifndef CRC16_H
#define CRC16_H
#include "type.h"

#define MASK 0x1021

union U16_BYTE {
    u16 uData;
    unsigned char cArray[2];
};


u32 crc16_le(u32 size, u8 const *data);


void GenerateCRC16Ex(
      const unsigned char*  iArray,			/* Array for the Data [in] */
      unsigned int          iSize,			/* Length of the array for the Data [in] */
      unsigned char*        ioArray,		/* Array for the Data [in, out] */
      unsigned int&         oSize           /* Length of the Data [out] */
      );

#endif // CRC16_H

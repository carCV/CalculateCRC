#include "crc32.h"


u32  crc32_le(u32 crc, unsigned char const *p, unsigned int len)
{
    int i;
    while (len--) {
        crc ^= *p++;
        for (i = 0; i < 8; i++)
            crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
    }
    return crc;
}


//计算文件的CRC值
VKeyGenResultEx GenerateCRCEx(
      const unsigned char*  iArray,			/* Array for the Data [in] */
      unsigned int          iSize,			/* Length of the array for the Data [in] */
      unsigned char*        ioArray,		/* Array for the Data [in, out] */
      unsigned int&         oSize           /* Length of the Data [out] */
      )
{
    UN_INTBYTE u1;


    unsigned int r1 = crc32_le(0xffffffff, iArray, iSize);
    r1 = r1 ^ 0xffffffff;

    u1.uData = r1;
    ioArray[0] = u1.cArray[3];
    ioArray[1] = u1.cArray[2];
    ioArray[2] = u1.cArray[1];
    ioArray[3] = u1.cArray[0];
    oSize = 4;

    return KGRE_Ok;
}



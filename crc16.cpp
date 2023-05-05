#include "crc16.h"

u32 crc16_le(u32 size, u8 const *data) {
    u32 crc = 0xffff; /* initial value */
    u32 tmp;
    for(u32 i = 0; i < size; i++) {
        tmp = data[i] << 8;
        for(u8 j = 0;j < 8; j++) {
            if((crc ^ tmp) & 0x8000) {
                crc = (crc<<1) ^ MASK;
            } else {
                crc <<= 1;
            }
            tmp <<= 1;
        }
    }
    return crc;
}



// 计算文件的CRC值
void GenerateCRC16Ex(
      const unsigned char*  iArray,			/* Array for the Data [in] */
      unsigned int          iSize,			/* Length of the array for the Data [in] */
      unsigned char*        ioArray,		/* Array for the Data [in, out] */
      unsigned int&         oSize           /* Length of the Data [out] */
      )
{
    U16_BYTE u1;
    u32 r1 = crc16_le(iSize, iArray);
    u1.uData = r1;

    ioArray[0] = u1.cArray[1];
    ioArray[1] = u1.cArray[0];
    oSize = 2;
}

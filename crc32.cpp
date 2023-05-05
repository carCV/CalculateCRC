#include "crc32.h"


u32 crc32_le(u32 size, u8 const *data)
{
    u32 crc = 0xffffffff; /* initial value */
    while (size--) {
        crc ^= *data++;
        for (u32 i = 0; i < 8; i++)
            crc = (crc >> 1) ^ ((crc & 1) ? CRC32_POLY_LE : 0);
    }
    return ~crc; // 输出数据翻转
}


// 计算文件的CRC值
void GenerateCRC32Ex(
      const unsigned char*  iArray,			/* Array for the Data [in] */
      unsigned int          iSize,			/* Length of the array for the Data [in] */
      unsigned char*        ioArray,		/* Array for the Data [in, out] */
      unsigned int&         oSize           /* Length of the Data [out] */
      )
{
    U32_BYTE u1;
    u32 r1 = crc32_le(iSize, iArray);

    u1.uData = r1;
    ioArray[0] = u1.cArray[3];
    ioArray[1] = u1.cArray[2];
    ioArray[2] = u1.cArray[1];
    ioArray[3] = u1.cArray[0];
    oSize = 4;

}



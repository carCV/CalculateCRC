#ifndef CRC32_H
#define CRC32_H

#define CRCPOLY_LE 0xedb88320

typedef unsigned int u32;
typedef unsigned char u8;


enum VKeyGenResultEx
{
  KGRE_Ok = 0,
  KGRE_BufferToSmall = 1,
  KGRE_SecurityLevelInvalid = 2,
  KGRE_VariantInvalid = 3,
  KGRE_UnspecifiedError = 4
};

union UN_INTBYTE
{
    u32 uData;
    unsigned char cArray[4];
};

u32  crc32_le(u32 crc, unsigned char const *p, unsigned int len);

VKeyGenResultEx GenerateCRCEx(
      const unsigned char*  iArray,			/* Array for the Data [in] */
      unsigned int          iSize,			/* Length of the array for the Data [in] */
      unsigned char*        ioArray,		/* Array for the Data [in, out] */
      unsigned int&         oSize           /* Length of the Data [out] */
      );



#endif // CRC32_H

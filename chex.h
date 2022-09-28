#ifndef CHEX_H
#define CHEX_H
#include <QFile>

const quint8 MIN_HEX_LINE_COUNT_LENGHT = 12;

typedef enum _tagHexErrorCode {
    HEX_NO_ERROR = 0,
    HEX_FORMAT_ERROR,
    HEX_VERIFY_ERROR,
    HEX_LENGHT_ERROR,
    HEX_USERPAPR_EEROR
}HexErrorCode;

typedef enum _HexRecordType
{
    RECORD_DATA = 0,
    RECORD_END_OF_FILE,
    RECORD_EXTENDED_SEGMENT_ADDRESS,
    RECORD_START_SEGMENT_ADDRESS,
    RECORD_EXTENDED_LINEAR_ADDRESS,
    RECORD_START_LINEAR_ADDRESS,
    RECORD_HEX_MAX
}HexType;

typedef struct _HexLineData
{
    HexType   type;
    quint8      count;
    quint32     address;
    quint8      data[80];
    quint8      checksum;
    quint8      datalen;
}HexLineData;

class CHex
{
public:
    CHex();
    HexErrorCode getHexLineData(QByteArray bydata,HexLineData *p);
private:
    char convertHexChar(char ch);

};



#endif // CHEX_H

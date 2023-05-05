#ifndef CS19_H
#define CS19_H

#include <QFile>

const quint8 MAX_S19_LINE_COUNT_LENGHT = 76;
const quint8 MIN_S19_LINE_COUNT_LENGHT = 10;    //S9xx0000yy
const quint8 MAX_S19_LINE_LENGHT = 156;

typedef enum _S19ErrorCode
{
    S19_No_Error = 0,
    S19_Format_Error = 1,
    S19_Verify_Error = 2,
    S19_Lenght_Error = 3,
    S19_Userpapr_Error

} ErrorCode;

typedef enum _S19RecordType
{
    S0 = 0,
    S1 = 1,
    S2 = 2,
    S3 = 3,
    S5 = 4,
    S7 = 5,
    S8 = 6,
    S9 = 7,
    S_MAX

} RecordType;

typedef struct _S19LineData
{
    RecordType type;
    quint8 count;
    quint32 address;
    quint8 data[100]; //预留100个字节
    quint8 checksum;
    quint8 datalen;

} S19LineData;

class CS19
{
public:
    CS19();
    ErrorCode getS19LineData(QByteArray data,S19LineData *p);

private:
    char convertHexChar(char ch);
};




#endif // CS19_H


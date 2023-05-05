#include "cs19.h"

const QString S19TypeTable[8] =
{
    "S0","S1","S2","S3","S5","S7","S8","S9"
};

CS19::CS19()
{

}

/**
 * 将字符转换为16进制字符
 * @brief CS19::convertHexChar
 * @param ch
 * @return
 */
char CS19::convertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return (ch-0x30);
    else if ((ch >= 'A') && (ch <= 'F'))
        return ((ch-'A') + 10);
    else if((ch >= 'a') && (ch <= 'f'))
        return ((ch-'a') + 10);
    else
        return -1;
}

/**
 * 获取S19文件中每行记录的具体内容
 * @brief CS19::getS19LineData
 * @param sourceData
 * @param p
 * @return
 */
ErrorCode CS19::getS19LineData(QByteArray sourceData, S19LineData *p)
{
    quint16 i = 0;
    quint8 cs_temp = 0;
//    QString qstr = (QString)data;
    QString qstr(sourceData);
    char * pcData = sourceData.data();
    quint32 lineLen = qstr.size();

    if(lineLen < MIN_S19_LINE_COUNT_LENGHT)
    {
        qDebug("S19_Lenght_Error");
        return S19_Lenght_Error;
    }

    //获取RecordType
    QString stype = qstr.mid(0,2); //截取字符串
    for(i = 0; i < S_MAX; i++)
    {
        if(stype == S19TypeTable[i])
        {
            p->type = (RecordType)i;
            break;
        }
    }
    if(i == S_MAX)
    {
        qDebug("S19_Format_Error");
        return S19_Format_Error;
    }

    // 获取count
    p->count = (convertHexChar(*(pcData + 2)) << 4) | convertHexChar(*(pcData + 3));
    cs_temp += p->count;
    if(p->count != (lineLen / 2) - 3)
    {
        qDebug("S19_Format_Error");
        return S19_Format_Error;
    }

    //获取address
    if(p->type == S0)   //2 Bytes address,用0x0000填充
    {
        p->address = 0x0;
    }
    else if ((p->type == S1) || (p->type == S5) || (p->type == S9)) //2 Bytes address
    {
        p->address = (convertHexChar(*(pcData + 4)) << 12) |
                     (convertHexChar(*(pcData + 5)) << 8) |
                     (convertHexChar(*(pcData + 6)) << 4) |
                     (convertHexChar(*(pcData + 7)));

        cs_temp += (p->address >> 8) & 0x0FF;
        cs_temp += p->address & 0x0FF;

        //获取data
        for(i = 0; i < p->count - 3; i++)
        {
            p->data[i] = (convertHexChar(*(pcData + 2*i + 8)) << 4) | convertHexChar(*(pcData + 2*i + 9));
            cs_temp += p->data[i];
        }

        //获取checksum
        p->checksum = (convertHexChar(*(pcData + 2*i + 8)) << 4) | convertHexChar(*(pcData + 2*i + 9));
        if(p->checksum != (0xFF - cs_temp))
        {
            qDebug("S19_Verify_Error");
            return S19_Verify_Error;
        }
    }
    else if((p->type == S2) || (p->type == S8))  //3 Bytes address
    {
        p->address = (convertHexChar(*(pcData + 4)) << 20) |
                     (convertHexChar(*(pcData + 5)) << 16) |
                     (convertHexChar(*(pcData + 6)) << 12) |
                     (convertHexChar(*(pcData + 7)) << 8) |
                     (convertHexChar(*(pcData + 8)) << 4) |
                     (convertHexChar(*(pcData + 9)));

        cs_temp += (p->address >> 16) & 0x0FF;
        cs_temp += (p->address >> 8) & 0x0FF;
        cs_temp += p->address & 0x0FF;

        //获取data
        for(i = 0; i < p->count - 4; i++)
        {
            p->data[i] = (convertHexChar(*(pcData + 2*i + 10)) << 4) | convertHexChar(*(pcData + 2*i + 11));
            cs_temp += p->data[i];
        }

        //获取checksum
        p->checksum = (convertHexChar(*(pcData + 2*i + 10)) << 4) | convertHexChar(*(pcData + 2*i + 11));
        if(p->checksum != (0xFF - cs_temp))
        {
            qDebug("S19_Verify_Error");
            return S19_Verify_Error;
        }
    }
    else if((p->type == S3) || (p->type == S7))  //4 Bytes address
    {
        p->address = (convertHexChar(*(pcData + 4)) << 28) |
                     (convertHexChar(*(pcData + 5)) << 24) |
                     (convertHexChar(*(pcData + 6)) << 20) |
                     (convertHexChar(*(pcData + 7)) << 16) |
                     (convertHexChar(*(pcData + 8)) << 12) |
                     (convertHexChar(*(pcData + 9)) << 8) |
                     (convertHexChar(*(pcData + 10)) << 4) |
                     (convertHexChar(*(pcData + 11)));

        cs_temp += (p->address >> 24) & 0x0FF;
        cs_temp += (p->address >> 16) & 0x0FF;
        cs_temp += (p->address >> 8) & 0x0FF;
        cs_temp += p->address & 0x0FF;

        //获取data
        for(i = 0; i < p->count - 5; i++)
        {
            p->data[i] = (convertHexChar(*(pcData + 2*i + 12)) << 4) | convertHexChar(*(pcData + 2*i + 13));
            cs_temp += p->data[i];
        }

        //获取checksum
        p->checksum = (convertHexChar(*(pcData + 2*i + 12)) << 4) | convertHexChar(*(pcData + 2*i + 13));
        if(p->checksum != (0xFF - cs_temp))
        {
            qDebug("S19_Verify_Error");
            return S19_Verify_Error;
        }
    }

    //获取datalen
    if(p->type == S1) p->datalen = p->count - 3;
    else if(p->type == S2) p->datalen = p->count - 4;
    else if(p->type == S3) p->datalen = p->count - 5;
    else p->datalen = 0;

    return S19_No_Error;

}

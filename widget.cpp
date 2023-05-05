#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

#include "crc16.h"
#include "crc32.h"



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

char Widget::convertChar2Byte(char ch)
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

quint32 records;
QString checkedData;
QVector<unsigned char> vtAllfileData;


void Widget::on_selectBtn_clicked()
{
    // 为了记住上次打开的路径
    QSettings setting("./Setting.ini",QSettings::IniFormat);
    QString lastPath = setting.value("LastFilePath").toString();

    // 每次选择文件时都要将vtAllfileData中原有的数据清除
    vtAllfileData.clear();

    QString path = QFileDialog::getOpenFileName(this,"open file dialog",lastPath,"files(*.s19 *.hex)");
    ui->pathLineEdit->setText(path);

    QByteArray context;
    QFile file(path);
    QFileInfo info(path);

    if(path.isEmpty() == false)
    {
        bool isOk = file.open(QIODevice::ReadOnly);

        if(ui->fileType->currentText().compare(info.suffix(),Qt::CaseInsensitive) != 0)
        {
            QMessageBox::warning(this,"FileTypeError","选择文件类型错误");
            isOk = false;
            ui->pathLineEdit->setText("请打开相应的" + ui->fileType->currentText() + "文件");
            ui->crcLineEdit->clear();
            ui->recordsLineEdit->clear();
            ui->textEdit->setEnabled(false);
            file.close();
        }
        else {
            ui->textEdit->setEnabled(true);
        }

        if(isOk == true)
        {
            for (records = 0; !file.atEnd(); records++)
            {
                context += file.readLine();
            }
            ui->textEdit->setText(context);
            ui->recordsLineEdit->setText(QString::number(records,10));
        }
        file.close();


        // 预留空间用于存储待校验的数据
        vtAllfileData.reserve(context.size());
        // 查看预留了多大的向量空间
//        qDebug() << "Reserve Qvector =" << context.size() << "byte";

        // 获取文本编辑区内容
        QString str = ui->textEdit->toPlainText();
        QStringList sections = str.split('\n');

        // 每行记录类型
        QString type;
        unsigned char * pcData;
        quint8 count;

        /**
         * 从文本编辑区中提取出待校验的S19文件数据，主要包括：S1、S2、S3的data场
         * S19文件格式：Sn + count + address + data + checksum
         *
         * 从文本编辑区中提取出待校验的Hex文件数据，主要包括：
         * Hex文件格式：: + count + address + type + data + checksum
         *
         */
        if(info.suffix().endsWith("s19",Qt::CaseInsensitive))
        {
            for(quint32 i = 0; i < records; i++)
            {
                checkedData = sections.at(i);
                type = checkedData.mid(0,2);  //获取S19文件对应的type
                pcData = (unsigned char*)checkedData.toLocal8Bit().data();
                count = (convertChar2Byte(*(pcData + 2)) << 4) | convertChar2Byte(*(pcData + 3));

                if(type == "S0")
                {
                    vtAllfileData.clear();
                }
                else if(type == "S1")
                {
                    for(qint8 j = 0; j < count-3; j++)
                    {
                        vtAllfileData.insert(vtAllfileData.end(),(convertChar2Byte(*(pcData + 2*j + 8)) << 4) | convertChar2Byte(*(pcData + 2*j + 9)));
                    }
//                  checkedDatas.append(checkedData.mid(8,count*2-4-2));
                }
                else if (type == "S2")
                {
                    for(quint8 j = 0; j < count-4; j++)
                    {
                        vtAllfileData.insert(vtAllfileData.end(),(convertChar2Byte(*(pcData + 2*j + 10)) << 4) | convertChar2Byte(*(pcData + 2*j + 11)));
                    }
//                  checkedDatas.append(checkedData.mid(10,count*2-6-2));
                }
                else if(type == "S3")
                {
                    for(quint8 j = 0; j < count-5; j++)
                    {
                        vtAllfileData.insert(vtAllfileData.end(),(convertChar2Byte(*(pcData + 2*j + 12)) << 4) | convertChar2Byte(*(pcData + 2*j + 13)));
                    }
//                  checkedDatas.append(checkedData.mid(12,count*2-8-2));
                }
             }
        }
        else if(info.suffix().endsWith("hex",Qt::CaseInsensitive))
        {
            for(quint32 i = 0; i < records; i++)
            {
                checkedData = sections.at(i);
                // 获取Hex文件对应的type
                type = checkedData.mid(7,2);
                pcData = (unsigned char*)checkedData.toLocal8Bit().data();
                count = (convertChar2Byte(*(pcData + 1)) << 4) | convertChar2Byte(*(pcData +2));

                if(type == "00")
                {
                    for(quint16 j = 0; j < count; j++)
                    {
                        vtAllfileData.insert(vtAllfileData.end(),(convertChar2Byte(*(pcData + 2*j + 9)) << 4) | convertChar2Byte(*(pcData + 2*j + 10)));
                    }
                }

            }
        }

    }

}

void Widget::on_calculateBtn_clicked()
{
    unsigned char crcBuf[4] = {0};
    unsigned int crcLen = 0;
    QString crcValue;
    QString crcType = ui->crcType->currentText();

    qDebug() <<"CheckedData's size =" << vtAllfileData.size() << "byte";

    if(vtAllfileData.size() != 0)
    {
        // 计算crc16值
        if(crcType.compare("crc16") == 0) {
            GenerateCRC16Ex(&vtAllfileData[0], vtAllfileData.size(), crcBuf, crcLen);
        }
        // 计算crc32值
        else if (crcType.compare("crc32") == 0) {
            GenerateCRC32Ex(&vtAllfileData[0], vtAllfileData.size(), crcBuf, crcLen);
    //        qDebug() << "crcBuf[0] =" << QString::number(crcBuf[0],16);
    //        qDebug() << "crcBuf[1] =" << QString::number(crcBuf[1],16);
    //        qDebug() << "crcBuf[2] =" << QString::number(crcBuf[2],16);
    //        qDebug() << "crcBuf[3] =" << QString::number(crcBuf[3],16);
    //        qDebug() << "crcLen =" << crcLen;
        }

    }

//    QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0')); Qt数字字符串自动补0或空格的写法

    for(unsigned int i=0; i<crcLen; i++)
    {
        crcValue.append(QString("%1").arg(crcBuf[i]&0xFF,2,16,QLatin1Char('0')).toUpper());
    }

    ui->crcLineEdit->setText(crcValue);
}





#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

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

quint32 records;
QString checkedData;
QVector<unsigned char> vtAllfileData;



char ConvertChar2Byte(char ch)
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

void Widget::on_selectButton_clicked()
{
    //每次选择文件时都要将vtAllfileData中原有的数据清除
    vtAllfileData.clear();

    QString path = QFileDialog::getOpenFileName(this,"open file dialog","/","files(*.s19 *.hex)");
    ui->pathLineEdit->setText(path);


    if(path.isEmpty() == false)
    {
        QFile file(path);
        QFileInfo info(path);
        QByteArray context;

        bool isOk = file.open(QIODevice::ReadOnly);

        if(ui->comboBox->currentText().compare(info.suffix(),Qt::CaseInsensitive) != 0)
        {
            QMessageBox::warning(this,"FileTypeError","选择文件类型错误");
            isOk = false;
            ui->pathLineEdit->setText("请打开相应的" + ui->comboBox->currentText() + "文件");
            ui->recordsLineEdit->clear();
            file.close();
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


        //预留空间用于存储待校验的数据
        vtAllfileData.reserve(context.size());

        //查看预留了多大的向量空间
        qDebug() << "Reserve Qvector =" << context.size() << "byte";

        //获取文本编辑区内容
        QString str = ui->textEdit->toPlainText();
        QStringList sections = str.split('\n');


        QString type;   //记录类型
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
        if(info.suffix().endsWith("S19",Qt::CaseInsensitive))
        {
            for(quint32 i = 0; i < records; i++)
            {
                checkedData = sections.at(i);
                type = checkedData.mid(0,2);  //获取S19文件对应的type
                pcData = (unsigned char*)checkedData.toLocal8Bit().data();
                count = (ConvertChar2Byte(*(pcData + 2)) << 4) | ConvertChar2Byte(*(pcData + 3));

                if(type == "S0")
                {
                    vtAllfileData.clear();
//                  checkedDatas = "";
                }
                else if(type == "S1")
                {
                    for(qint8 j = 0; j < count-3; j++)
                    {
                        vtAllfileData.insert(vtAllfileData.end(),(ConvertChar2Byte(*(pcData + 2*j + 8)) << 4) | ConvertChar2Byte(*(pcData + 2*j + 9)));
                    }

//                  checkedDatas.append(checkedData.mid(8,count*2-4-2));
                }
                else if (type == "S2")
                {
                    for(quint8 j = 0; j < count-4; j++)
                    {
                        vtAllfileData.insert(vtAllfileData.end(),(ConvertChar2Byte(*(pcData + 2*j + 10)) << 4) | ConvertChar2Byte(*(pcData + 2*j + 11)));
                    }

//                  checkedDatas.append(checkedData.mid(10,count*2-6-2));
                }
                else if(type == "S3")
                {
                    for(quint8 j = 0; j < count-5; j++)
                    {
                        vtAllfileData.insert(vtAllfileData.end(),(ConvertChar2Byte(*(pcData + 2*j + 12)) << 4) | ConvertChar2Byte(*(pcData + 2*j + 13)));
                    }
//                  checkedDatas.append(checkedData.mid(12,count*2-8-2));
                }
             }
        }
        else if(info.suffix().endsWith("Hex",Qt::CaseInsensitive))
        {
            for(quint32 i = 0; i < records; i++)
            {
                checkedData = sections.at(i);
                type = checkedData.mid(7,2);  //获取Hex文件对应的type
                pcData = (unsigned char*)checkedData.toLocal8Bit().data();
                count = (ConvertChar2Byte(*(pcData + 1)) << 4) | ConvertChar2Byte(*(pcData +2));

                if(type == "00")
                {
                    for(quint16 j = 0; j < count; j++)
                    {
                        vtAllfileData.insert(vtAllfileData.end(),(ConvertChar2Byte(*(pcData + 2*j + 9)) << 4) | ConvertChar2Byte(*(pcData + 2*j + 10)));
                    }
                }

            }
        }

    }
}

void Widget::on_calculateButton_clicked()
{
    unsigned char crcBuf[4] = {0};
    quint32 crcLen = 0;

    qDebug() <<"CheckedData's size =" << vtAllfileData.size() << "byte";

    if(vtAllfileData.size() != 0)
    {
        GenerateCRCEx(&vtAllfileData[0],vtAllfileData.size(),crcBuf,crcLen);
//        qDebug() << "crcBuf[0] =" << QString::number(crcBuf[0],16);
//        qDebug() << "crcBuf[1] =" << QString::number(crcBuf[1],16);
//        qDebug() << "crcBuf[2] =" << QString::number(crcBuf[2],16);
//        qDebug() << "crcBuf[3] =" << QString::number(crcBuf[3],16);
//        qDebug() << "crcLen =" << crcLen;
    }


    QString crcValue;
//    QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0')); Qt数字字符串自动补0或空格

    for(qint32 i=0; i<crcLen; i++)
    {

        crcValue.append(QString("%1").arg(crcBuf[i]&0xFF,2,16,QLatin1Char('0')).toUpper());

    }

    ui->crcLineEdit->setText(crcValue);



}





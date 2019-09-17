#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>
#include "cs19.h"

namespace Ui {
class Widget;
}

char ConvertChar2Byte(char ch);

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:

    void on_selectButton_clicked();

    void on_calculateButton_clicked();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H

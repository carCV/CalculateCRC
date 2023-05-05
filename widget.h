#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    char convertChar2Byte(char ch);

private slots:
    void on_selectBtn_clicked();
    void on_calculateBtn_clicked();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H

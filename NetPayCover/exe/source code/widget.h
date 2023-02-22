#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void beforeTax(int num,double Insurance,double&sala,double&Tax);
    void afterTax(int num,double Insurance,double&sala,double&Tax);
    ~Widget();
private slots:
    void on_pushButton_clicked();

    void on_More_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H

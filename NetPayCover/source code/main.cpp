#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("工资计算");
    w.setFixedSize(700,590);
    w.show();
    return a.exec();
}

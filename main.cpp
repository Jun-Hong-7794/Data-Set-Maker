#include "data_set_maker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Data_Set_Maker w;
    w.show();

    return a.exec();
}

#include "music.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    music w;
    w.show();
    return a.exec();
}

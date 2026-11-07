#include "UNAPulpe.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    UNAPulpe window;
    window.show();
    return app.exec();
}

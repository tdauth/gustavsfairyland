#include <QtWidgets/QApplication>
#include "fairytale.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    fairytale fairytale;
    fairytale.show();
    return app.exec();
}

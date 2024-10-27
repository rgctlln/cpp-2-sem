#include "MenuWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool debug = false;
    if (argc == 2 && QString(argv[1]) == "dbg")
    {
        debug = true;
    }

    MenuWindow menuWindow;
    if (debug)
    {
        menuWindow.enable_debug_mode();
    }
    menuWindow.show();

    return a.exec();
}

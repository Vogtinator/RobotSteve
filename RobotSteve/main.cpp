#include <iostream>
#include <QApplication>

#include "mainwindow.h"
#include "steveinterpreter.h"

int main(int argc, char *argv[])
{
    try {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
    }
    catch (SteveInterpreterException e) {
        std::cerr << e.what() << std::endl;
    }
    catch (QString s) {
        std::cerr << s.toStdString() << std::endl
                  << "Bye :-/" << std::endl;
    }
    catch (const char *e) {
        std::cerr << e << std::endl
                  << "Bye :-/" << std::endl;
    }
    catch (...) {
        std::cerr << "Umm, not good. Bye :-/" << std::endl;
    }
}

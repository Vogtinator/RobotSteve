#include <iostream>
#include <QApplication>
#include <QTextCodec>

#include "mainwindow.h"
#include "steveinterpreter.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

    try {
    QApplication a{argc, argv};
    QCoreApplication::setOrganizationName("FDG AB");
    QCoreApplication::setOrganizationDomain("fdg-ab.de");
    QCoreApplication::setApplicationName("Robot Steve");

    MainWindow w;
    w.show();

    return a.exec();
    }
    catch (SteveInterpreterException &e) {
        std::cerr << e.what() << std::endl
                  << "Bye :-/" << std::endl;
    }
    catch (QString &s) {
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

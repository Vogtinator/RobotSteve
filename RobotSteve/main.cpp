/*
 * Author: Fabian Vogt
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 * or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 * Use in public and private schools for educational purposes strongly permitted!
 */

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

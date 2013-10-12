#include <iostream>
#include <QApplication>
#include <QTextCodec>
#include <QStyleFactory>
#include <QFileInfo>
#include <QMessageBox>

#include "mainwindow.h"
#include "steveinterpreter.h"

enum ARG_PARSE_STATE {
    NEXT_IS_SOMETHING,
    NEXT_IS_CODE,
    NEXT_IS_WORLD
};

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

    ARG_PARSE_STATE state = NEXT_IS_SOMETHING;
    QString code_file, world_file;

    for(int i = 1; i < QCoreApplication::arguments().length(); i++)
    {
        const QString argument = QCoreApplication::arguments()[i];

        if(state == NEXT_IS_CODE)
        {
            if(!code_file.isEmpty())
            {
                QMessageBox::critical(nullptr, QObject::trUtf8("Fehler"), QObject::trUtf8("Bitte nur einmal Code laden."));
                return 1;
            }

            code_file = argument;

            state = NEXT_IS_SOMETHING;
        }
        else if(state == NEXT_IS_WORLD)
        {
            if(!world_file.isEmpty())
            {
                QMessageBox::critical(nullptr, QObject::trUtf8("Fehler"), QObject::trUtf8("Bitte nur eine Welt laden."));
                return 1;
            }

            world_file = argument;

            state = NEXT_IS_SOMETHING;
        }
        else
        {
            if(argument.compare("--dark", Qt::CaseInsensitive) == 0)
            {
                QPalette palette;
                palette.setColor(QPalette::Window, QColor(53,53,53));
                palette.setColor(QPalette::WindowText, Qt::white);
                palette.setColor(QPalette::Base, QColor(15,15,15));
                palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
                palette.setColor(QPalette::ToolTipBase, Qt::white);
                palette.setColor(QPalette::ToolTipText, Qt::white);
                palette.setColor(QPalette::Text, Qt::white);
                palette.setColor(QPalette::Button, QColor(53,53,53));
                palette.setColor(QPalette::ButtonText, Qt::white);
                palette.setColor(QPalette::BrightText, Qt::red);
                palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
                palette.setColor(QPalette::HighlightedText, Qt::black);
                qApp->setPalette(palette);
            }
            else if(argument.compare("--version", Qt::CaseInsensitive) == 0)
            {
                std::cout << "Robot Steve 0.9.7" << std::endl;

                return 0;
            }
            else if(argument.compare("--code", Qt::CaseInsensitive) == 0)
                state = NEXT_IS_CODE;

            else if(argument.compare("--world", Qt::CaseInsensitive) == 0)
                state = NEXT_IS_WORLD;

            else //Filename (code/world)
            {
                if(code_file.isEmpty() && !world_file.isEmpty())
                    code_file = argument;
                else if(!code_file.isEmpty() && world_file.isEmpty())
                    world_file = argument;
                else //Guess
                {
                    QFileInfo file_info{argument};

                    if(file_info.completeSuffix().compare("stworld", Qt::CaseInsensitive) == 0)
                        world_file = argument;

                    else
                        code_file = argument;
                }
            }
        }
    }

    MainWindow w;
    if(!code_file.isEmpty())
    {
        if(code_file.startsWith(':'))
        {
            QMessageBox::critical(nullptr, QObject::trUtf8("Fehler"), QObject::trUtf8("Bitte kein Doppelpunkt an erster Stelle."));
            return 1;
        }

        w.loadCodeFile(code_file);
    }

    if(!world_file.isEmpty())
    {
        if(world_file.startsWith(':'))
        {
            QMessageBox::critical(nullptr, QObject::trUtf8("Fehler"), QObject::trUtf8("Bitte kein Doppelpunkt an erster Stelle."));
            return 1;
        }

        w.loadWorldFile(world_file);
    }

    w.show();

    return a.exec();
    }
    catch (std::exception &e) {
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

    return 1;
}

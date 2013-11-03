#include <iostream>
#include <QFile>
#include <QMetaEnum>
#include <QTextDocument>
#include <QMessageBox>

#include "stevehelp.h"
#include "helpdialog.h"

SteveHelp::SteveHelp(SteveInterpreter *interpreter)
    : interpreter{interpreter}
{
}

SteveHelp::SteveHelp(SteveInterpreter *interpreter, QString path)
    : interpreter{interpreter}
{
    if(!loadFile(path))
        QMessageBox::critical(nullptr, QObject::trUtf8("Fehler beim Laden"), QObject::trUtf8("Die Hilfe konnte nicht geladen werden."));
}

bool SteveHelp::loadFile(QString path)
{
    QFile help_file{path};
    if(!help_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QXmlStreamReader help_reader{&help_file};

    QMetaEnum keyword_meta = SteveInterpreter::staticMetaObject.enumerator(SteveInterpreter::staticMetaObject.indexOfEnumerator("KEYWORD"));
    QMetaEnum instruction_meta = SteveInterpreter::staticMetaObject.enumerator(SteveInterpreter::staticMetaObject.indexOfEnumerator("INSTRUCTION"));
    QMetaEnum condition_meta = SteveInterpreter::staticMetaObject.enumerator(SteveInterpreter::staticMetaObject.indexOfEnumerator("CONDITION"));

    keyword_help.clear();
    instruction_help.clear();
    condition_help.clear();

    while(!help_reader.atEnd())
    {
        if(!help_reader.readNextStartElement())
            continue;

        QString name = help_reader.attributes().value("name").toString();
        if(name.isEmpty())
            continue;

        QStringList names = name.split(";");

        if(help_reader.name().compare("keyword", Qt::CaseInsensitive) == 0)
        {
            QString text = help_reader.readElementText(QXmlStreamReader::IncludeChildElements);

            for(const QString &n : names)
            {
                SteveInterpreter::KEYWORD i = static_cast<SteveInterpreter::KEYWORD>(keyword_meta.keyToValue(n.toStdString().c_str()));
                if(i == -1)
                {
                    std::cerr << QObject::trUtf8("Warnung: Unbekanntes Wort '%1'!").arg(n).toStdString() << std::endl;
                    continue;
                }

                keyword_help[i] = text;
            }
        }
        else if(help_reader.name().compare("instruction", Qt::CaseInsensitive) == 0)
        {
            QString text = help_reader.readElementText(QXmlStreamReader::IncludeChildElements);

            for(const QString &n : names)
            {
                SteveInterpreter::INSTRUCTION i = static_cast<SteveInterpreter::INSTRUCTION>(instruction_meta.keyToValue(n.toStdString().c_str()));
                if(i == -1)
                {
                    std::cerr << QObject::trUtf8("Warnung: Unbekanntes Wort '%1'!").arg(n).toStdString() << std::endl;
                    continue;
                }

                instruction_help[i] = text;
            }
        }
        else if(help_reader.name().compare("condition", Qt::CaseInsensitive) == 0)
        {
            QString text = help_reader.readElementText(QXmlStreamReader::IncludeChildElements);

            for(const QString &n : names)
            {
                SteveInterpreter::CONDITION i = static_cast<SteveInterpreter::CONDITION>(condition_meta.keyToValue(n.toStdString().c_str()));
                if(i == -1)
                {
                    std::cerr << QObject::trUtf8("Warnung: Unbekanntes Wort '%1'!").arg(n).toStdString() << std::endl;
                    continue;
                }

                condition_help[i] = text;
            }
        }
    }

    //Fill word_list
    int i;
    for(i = 0; i < keyword_meta.keyCount(); i++)
        word_list << interpreter->str(static_cast<SteveInterpreter::KEYWORD>(keyword_meta.value(i)));
    for(i = 0; i < instruction_meta.keyCount(); i++)
        word_list << interpreter->str(static_cast<SteveInterpreter::INSTRUCTION>(instruction_meta.value(i)));
    for(i = 0; i < condition_meta.keyCount(); i++)
        word_list << interpreter->str(static_cast<SteveInterpreter::CONDITION>(condition_meta.value(i)));

    return true;
}

QString SteveHelp::getHelp(SteveInterpreter::KEYWORD keyword)
{
    if(!keyword_help.contains(keyword))
        return QObject::trUtf8("Keine Hilfe für das Wort verfügbar.");

    return keyword_help[keyword];
}

QString SteveHelp::getHelp(SteveInterpreter::INSTRUCTION instruction)
{
    if(!instruction_help.contains(instruction))
        return QObject::trUtf8("Keine Hilfe für die Anweisung verfügbar.");

    return instruction_help[instruction];
}

QString SteveHelp::getHelp(SteveInterpreter::CONDITION condition)
{
    if(!condition_help.contains(condition))
        return QObject::trUtf8("Keine Hilfe für die Bedingung verfügbar.");

    return condition_help[condition];
}

QString SteveHelp::getHelp(QString word)
{
    SteveInterpreter::KEYWORD keyword = interpreter->getKeyword(word);
    SteveInterpreter::INSTRUCTION instruction = interpreter->getInstruction(word);
    SteveInterpreter::CONDITION condition = interpreter->getCondition(word);

    if(keyword != -1 && keyword_help.contains(keyword))
        return keyword_help[keyword];
    else if(instruction != -1 && instruction_help.contains(instruction))
        return instruction_help[instruction];
    else if(condition != -1 && condition_help.contains(condition))
        return condition_help[condition];
    else
        return QObject::trUtf8("Keine Hilfe für das Wort verfügbar.");
}

void SteveHelp::showHelp()
{
    QString content = QObject::trUtf8("<table border='2' cellspacing='0' cellpadding='10'><tr><td><h1>Wörter:</h1>");

    QMetaEnum keyword_meta = SteveInterpreter::staticMetaObject.enumerator(SteveInterpreter::staticMetaObject.indexOfEnumerator("KEYWORD"));
    QMetaEnum instruction_meta = SteveInterpreter::staticMetaObject.enumerator(SteveInterpreter::staticMetaObject.indexOfEnumerator("INSTRUCTION"));
    QMetaEnum condition_meta = SteveInterpreter::staticMetaObject.enumerator(SteveInterpreter::staticMetaObject.indexOfEnumerator("CONDITION"));

    for(int i = 0; i < keyword_meta.keyCount(); i++)
    {
        SteveInterpreter::KEYWORD t = static_cast<SteveInterpreter::KEYWORD>(keyword_meta.value(i));
        content.append(QString("<h2>%1</h2>%2").arg(interpreter->str(t)).arg(Qt::convertFromPlainText(getHelp(t))));
    }

    content.append(QObject::trUtf8("</td><td><h1>Anweisungen:</h1>"));

    for(int i = 0; i < instruction_meta.keyCount(); i++)
    {
        SteveInterpreter::INSTRUCTION t = static_cast<SteveInterpreter::INSTRUCTION>(instruction_meta.value(i));
        content.append(QString("<h2>%1</h2>%2").arg(interpreter->str(t)).arg(Qt::convertFromPlainText(getHelp(t))));
    }

    content.append(QObject::trUtf8("</td><td><h1>Bedingungen:</h1>"));

    for(int i = 0; i < condition_meta.keyCount(); i++)
    {
        SteveInterpreter::CONDITION t = static_cast<SteveInterpreter::CONDITION>(condition_meta.value(i));
        content.append(QString("<h2>%1</h2>%2").arg(interpreter->str(t)).arg(Qt::convertFromPlainText(getHelp(t))));
    }

    content.append(QString("</td></tr></table>"));

    HelpDialog hd{content, interpreter};

    hd.show();

    hd.exec();
}

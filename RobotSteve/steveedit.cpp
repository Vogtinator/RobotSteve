#include <iostream>
#include <QTextDocumentFragment>
#include <QTimer>
#include <QXmlStreamReader>
#include <QMetaEnum>
#include <QToolTip>

#include "steveedit.h"
#include "steveinterpreter.h"

SteveEdit::SteveEdit(SteveInterpreter *interpreter, QWidget *parent) :
    QPlainTextEdit(parent), interpreter{interpreter}
{
    QFile help_file{":/help/help.xml"};
    if(!help_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cerr << QObject::trUtf8("Hilfedatei konnte nicht geöffnet werden!").toStdString() << std::endl;
        return;
    }

    QXmlStreamReader help_reader{&help_file};

    QMetaEnum keyword_meta = SteveInterpreter::staticMetaObject.enumerator(SteveInterpreter::staticMetaObject.indexOfEnumerator("KEYWORD"));
    QMetaEnum instruction_meta = SteveInterpreter::staticMetaObject.enumerator(SteveInterpreter::staticMetaObject.indexOfEnumerator("INSTRUCTION"));
    QMetaEnum condition_meta = SteveInterpreter::staticMetaObject.enumerator(SteveInterpreter::staticMetaObject.indexOfEnumerator("CONDITION"));

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
}

void SteveEdit::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_F1)
    {
        QTextCursor cursor = textCursor();
        cursor.select(QTextCursor::WordUnderCursor);
        QString word = cursor.selection().toPlainText();
        SteveInterpreter::KEYWORD keyword = interpreter->getKeyword(word);
        SteveInterpreter::INSTRUCTION instruction = interpreter->getInstruction(word);
        SteveInterpreter::CONDITION condition = interpreter->getCondition(word);

        QString description = trUtf8("Keine Hilfe gefunden");

        if(keyword != -1 && keyword_help.contains(keyword))
            description = keyword_help[keyword];
        else if(instruction != -1 && instruction_help.contains(instruction))
            description = instruction_help[instruction];
        else if(condition != -1 && condition_help.contains(condition))
            description = condition_help[condition];

        QPoint global = mapToGlobal({this->cursorRect().x(), this->cursorRect().y() + this->cursorRect().height()});


        QToolTip::showText(global, QString("<b>%1</b>%2").arg(word).arg(Qt::convertFromPlainText(description)), this);
    }
    else if(e->key() == Qt::Key_Escape)
        QToolTip::hideText();

    else
        QPlainTextEdit::keyPressEvent(e);
}

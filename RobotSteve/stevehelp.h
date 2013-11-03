#ifndef STEVEHELP_H
#define STEVEHELP_H

#include <QString>
#include <QHash>
#include <QStringList>

#include "steveinterpreter.h"

class SteveHelp
{
public:
    SteveHelp(SteveInterpreter *interpreter);
    SteveHelp(SteveInterpreter *interpreter, QString path);

    bool loadFile(QString path);
    QString getHelp(SteveInterpreter::KEYWORD keyword);
    QString getHelp(SteveInterpreter::INSTRUCTION instruction);
    QString getHelp(SteveInterpreter::CONDITION condition);
    QString getHelp(QString word); //If word is unknown, an error message is returned.

    QStringList &getWordList() { return word_list; }

    void showHelp();

private:
    SteveInterpreter *interpreter;
    QHash<SteveInterpreter::KEYWORD, QString> keyword_help;
    QHash<SteveInterpreter::INSTRUCTION, QString> instruction_help;
    QHash<SteveInterpreter::CONDITION, QString> condition_help;
    QStringList word_list;
};

#endif // STEVEHELP_H

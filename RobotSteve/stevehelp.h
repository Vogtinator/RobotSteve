#ifndef STEVEHELP_H
#define STEVEHELP_H

#include <QString>
#include <QHash>

#include "steveinterpreter.h"

class SteveHelp
{
public:
    SteveHelp(SteveInterpreter *interpreter);

    bool loadFile(QString path);
    QString getHelp(SteveInterpreter::KEYWORD keyword);
    QString getHelp(SteveInterpreter::INSTRUCTION instruction);
    QString getHelp(SteveInterpreter::CONDITION condition);
    QString getHelp(QString word); //If word is unknown, an error message is returned.

    void showHelp();

private:
    SteveInterpreter *interpreter;
    QHash<SteveInterpreter::KEYWORD, QString> keyword_help;
    QHash<SteveInterpreter::INSTRUCTION, QString> instruction_help;
    QHash<SteveInterpreter::CONDITION, QString> condition_help;
};

#endif // STEVEHELP_H

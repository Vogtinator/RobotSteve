#include <QObject>
#include <QDebug>

#include "steveinterpreter.h"

SteveInterpreter::SteveInterpreter()
{
    keywords[KEYWORD_IF] = QObject::trUtf8("wenn");
    keywords[KEYWORD_THEN] = QObject::trUtf8("dann");
    keywords[KEYWORD_ELSE] = QObject::trUtf8("sonst");
    keywords[KEYWORD_REPEAT] = QObject::trUtf8("wiederhole");
    keywords[KEYWORD_TIMES] = QObject::trUtf8("mal");
    keywords[KEYWORD_WHILE] = QObject::trUtf8("solange");
    keywords[KEYWORD_DO] = QObject::trUtf8("tue");
    keywords[KEYWORD_NEW_INSTR] = QObject::trUtf8("anweisung");
    keywords[KEYWORD_NEW_COND] = QObject::trUtf8("bedingung");

    keywords[KEYWORD_IF_END] = "*" + keywords[KEYWORD_IF];
    keywords[KEYWORD_REPEAT_END] = "*" + keywords[KEYWORD_REPEAT];
    keywords[KEYWORD_WHILE_END] = "*" + keywords[KEYWORD_WHILE];
    keywords[KEYWORD_NEW_INSTR_END] = "*" + keywords[KEYWORD_NEW_INSTR];
    keywords[KEYWORD_NEW_COND_END] = "*" + keywords[KEYWORD_NEW_COND];
}

void SteveInterpreter::setCode(QStringList code) throw (QString)
{
    this->code = code;
    QStack<int> branch_entrys;
    QStack<BLOCK> block_types;
    current_line = code.size() - 1;
    //TODO: Eigene Klasse für Exceptions, Zeilenangabe!
    //TODO: Blöcke wie unten automatisieren, REPEAT ~ WHILE ~ IF (ohne ELSE) ~ INSTR ~ COND

    while(current_line >= 0)
    {
        QString line = code[current_line].simplified().toLower();

        if(line.startsWith(keywords[KEYWORD_IF]))
        {
            if(branch_entrys.size() && block_types.pop() == BLOCK_IF)
                branches[current_line] = branch_entrys.pop();
            else //TODO: Basierend auf block_types.pop sagen, was fehlt. Zeile: branches[branch_entrys.pop()]!
                throw QObject::trUtf8("Es fehlt ein %1!").arg(keywords[KEYWORD_IF_END]);
        }
        else if(line.startsWith(keywords[KEYWORD_IF_END]))
        {
            branch_entrys.push(current_line);
            block_types.push(BLOCK_IF);
        }
        //TODO: Zwei ELSE in IF ist illegal!
        else if(line.startsWith(keywords[KEYWORD_ELSE]))
        {
            if(branch_entrys.size() && block_types.pop() == BLOCK_IF)
                branches[current_line] = branch_entrys.pop();
            else
                throw QObject::trUtf8("Es fehlt ein %1!").arg(keywords[KEYWORD_IF_END]);

            branch_entrys.push(current_line);
            block_types.push(BLOCK_IF);
        }

        current_line--;
    }
}

void SteveInterpreter::reset()
{
    current_line = start_line;
    stack.clear();
}

void SteveInterpreter::executeLine() throw (QString)
{
    throw "Not implemented!";
}

int SteveInterpreter::getLine()
{
    return current_line;
}

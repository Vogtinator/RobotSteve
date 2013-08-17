#include <iostream>
#include <QObject>
#include <QDebug>

#include "steveinterpreter.h"

#define DEFAULT_BLOCK_KEYWORDS(name) { .begin = KEYWORD_##name, \
        .end = KEYWORD_##name##_END, \
        .type = BLOCK_##name, \
    }

static struct BlockKeywords blocks[] = {
    DEFAULT_BLOCK_KEYWORDS(IF),
    DEFAULT_BLOCK_KEYWORDS(REPEAT),
    DEFAULT_BLOCK_KEYWORDS(WHILE),
    DEFAULT_BLOCK_KEYWORDS(NEW_INSTR),
    DEFAULT_BLOCK_KEYWORDS(NEW_COND)
};

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

void SteveInterpreter::findAndThrowMissingBegin(int line, BLOCK block, QString affected) throw (SteveInterpreterException)
{
    for(auto block_keywords : blocks)
    {
        if(block_keywords.type == block || (block == BLOCK_ELSE && block_keywords.type == BLOCK_IF))
            throw SteveInterpreterException(QObject::trUtf8("Es fehlt ein %1.").arg(keywords[block_keywords.begin]), line, affected);
    }
}

void SteveInterpreter::setCode(QStringList code) throw (SteveInterpreterException)
{
    this->code = code;
    branches.clear();
    customConditions.clear();
    customInstructions.clear();
    QStack<int> branch_entrys;
    QStack<BLOCK> block_types;
    current_line = code.size() - 1;

    while(current_line >= 0)
    {
        QStringList line = code[current_line].simplified().toLower().split(" ", QString::SkipEmptyParts);
        if(line.size() == 0)
            goto end;

        //Special case, KEYWORD_ELSE has to be trated seperately
        if(line[0] == keywords[KEYWORD_ELSE])
        {
            if(branch_entrys.size())
            {
                BLOCK last_block = block_types.pop();
                if(last_block == BLOCK_IF)
                {
                    branches[current_line] = branch_entrys.pop();
                    branch_entrys.push(current_line);
                    block_types.push(BLOCK_ELSE);
                    goto end;
                }
                else
                {
                    findAndThrowMissingBegin(current_line, last_block, keywords[KEYWORD_ELSE]);
                    throw SteveInterpreterException("WTF #1", current_line);
                }
            }
            throw SteveInterpreterException(QObject::trUtf8("Es fehlt ein %1.").arg(keywords[KEYWORD_IF_END]), current_line, keywords[KEYWORD_ELSE]);
        }
        else
        {
            for(auto i : blocks)
            {
                if(line[0] == keywords[i.end])
                {
                    block_types.push(i.type);
                    branch_entrys.push(current_line);
                    break;
                }
                if(line[0] == keywords[i.begin])
                {
                    if(branch_entrys.size())
                    {
                        BLOCK last_block = block_types.pop();
                        if(last_block == i.type || (last_block == BLOCK_ELSE && i.type == BLOCK_IF))
                        {
                            branches[current_line] = branch_entrys.pop();
                            break;
                        }
                        else
                        {
                            findAndThrowMissingBegin(current_line, last_block, keywords[i.begin]);
                            throw SteveInterpreterException("WTF #1", current_line);
                        }
                    }
                    throw SteveInterpreterException(QObject::trUtf8("Es fehlt ein %1.").arg(keywords[i.end]), current_line, keywords[i.begin]);
                }
            }
        }

        end:
        current_line--;
    }

    if(branch_entrys.size())
    {
        findAndThrowMissingBegin(0, block_types.pop());
        throw SteveInterpreterException("WTF #2", 0);
    }

    //Now parse a second time
    current_line = 0;
    while(current_line < code.size())
    {
        QStringList line = code[current_line].simplified().toLower().split(" ", QString::SkipEmptyParts);
        if(line.size() == 0)
            goto end2;

        for(auto i : blocks)
        {
            if(line[0] == keywords[i.begin])
            {
                if(i.type == BLOCK_NEW_COND || i.type == BLOCK_NEW_INSTR)
                {
                    if(block_types.size() > 0)
                    {
                        BLOCK in = block_types.pop();
                        for(auto bk : blocks)
                            if(bk.type == in)
                                throw SteveInterpreterException(QObject::trUtf8("%1 ist nicht in einem %2-Block erlaubt.").arg(keywords[i.begin]).arg(keywords[bk.begin]), current_line);

                        throw SteveInterpreterException(QObject::trUtf8("WTF #5"), current_line);
                    }

                    if(line.size() == 1)
                        throw SteveInterpreterException(QObject::trUtf8("Bezeichnung fehlt."), current_line);
                    else if(line.size() > 2)
                    {
                        //TODO: affected
                        throw SteveInterpreterException(QObject::trUtf8("Zu viele Bezeichnungen."), current_line);
                    }

                    QMap<QString, int> *customSymbols = i.type == BLOCK_NEW_COND ? &customConditions : &customInstructions;
                    if(customSymbols->contains(line[1]))
                        throw SteveInterpreterException(QObject::trUtf8("%1 %2 existiert schon in Zeile %3").arg(keywords[i.begin]).arg(line[1]).arg((*customSymbols)[line[1]]), current_line, line[1]);

                    (*customSymbols)[line[1]] = current_line;
                }
                block_types.push(i.type);
                branch_entrys.push(current_line);
                break;
            }

            if(line[0] == keywords[i.end])
            {
                if(branch_entrys.size())
                {
                    BLOCK last_block = block_types.pop();
                    if(last_block == i.type)
                    {
                        branches[current_line] = branch_entrys.pop();
                        break;
                    }
                }
                throw SteveInterpreterException(QObject::trUtf8("WTF #3"), current_line);
            }
        }

        end2:
        current_line++;
    }

    if(branch_entrys.size())
        throw SteveInterpreterException("WTF #4", code.size() - 1);
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

void SteveInterpreter::dumpCode()
{
    int line = 0;
    while(line < code.size())
    {
        std::cout << line << ": " << code[line].toStdString();
        if(branches.contains(line))
            std::cout << " (" << branches[line] << ")";

        std::cout << std::endl;
        line++;
    }
    std::cout << std::endl;

    std::cout << QObject::trUtf8("Bedingungen: ").toStdString() << std::endl;
    for(auto i : customConditions.keys())
        std::cout << QObject::trUtf8("%1 in Zeile %2").arg(i).arg(customConditions[i]).toStdString() << std::endl;

    std::cout << std::endl;

    std::cout << QObject::trUtf8("Anweisungen: ").toStdString() << std::endl;
    for(auto i : customInstructions.keys())
        std::cout << QObject::trUtf8("%1 in Zeile %2").arg(i).arg(customInstructions[i]).toStdString() << std::endl;
}

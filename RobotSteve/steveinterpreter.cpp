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

    instructions[INSTR_STEP] = QObject::trUtf8("schritt");
    instructions[INSTR_TURNLEFT] = QObject::trUtf8("linksdrehen");
    instructions[INSTR_TURNRIGHT] = QObject::trUtf8("rechtsdrehen");
    instructions[INSTR_PUTDOWN] = QObject::trUtf8("hinlegen");
    instructions[INSTR_PICKUP] = QObject::trUtf8("aufheben");
    instructions[INSTR_MARK] = QObject::trUtf8("markesetzen");
    instructions[INSTR_UNMARK] = QObject::trUtf8("markelöschen");
    instructions[INSTR_WAIT] = QObject::trUtf8("warten");
    instructions[INSTR_TONE] = QObject::trUtf8("ton");
    instructions[INSTR_QUIT] = QObject::trUtf8("beenden");

    conditions[COND_ISWALL] = QObject::trUtf8("istwand");
    conditions[COND_NOTISWALL] = QObject::trUtf8("nichtistwand");
    conditions[COND_ISBRICK] = QObject::trUtf8("istziegel");
    conditions[COND_NOTISBRICK] = QObject::trUtf8("nichtistziegel");
    conditions[COND_MARKED] = QObject::trUtf8("istmarke");
    conditions[COND_NOTMARKED] = QObject::trUtf8("nichtistmarke");
    conditions[COND_ISNORTH] = QObject::trUtf8("istnorden");
    conditions[COND_ISSOUTH] = QObject::trUtf8("istsüden");
    conditions[COND_ISEAST] = QObject::trUtf8("istosten");
    conditions[COND_ISWEST] = QObject::trUtf8("istwesten");
    conditions[COND_ISFULL] = QObject::trUtf8("istvoll");
    conditions[COND_NOTISFULL] = QObject::trUtf8("nichtistvoll");
    conditions[COND_ISEMPTY] = QObject::trUtf8("istleer");
    conditions[COND_NOTISEMPTY] = QObject::trUtf8("nichtistleer");
    conditions[COND_HASBRICK] = QObject::trUtf8("hatziegel");
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
    QStack<int> branch_entrys;
    QStack<BLOCK> block_types;

    this->code = code;
    branches.clear();
    customConditions.clear();
    customInstructions.clear();

    current_line = code.size() - 1;
    for(current_line = code.size() - 1; current_line >= 0; current_line--)
    {
        QStringList line = code[current_line].simplified().split(" ", QString::SkipEmptyParts);
        if(line.size() == 0)
            continue;

        //Special case, KEYWORD_ELSE has to be trated seperately
        if(line[0].compare(keywords[KEYWORD_ELSE], Qt::CaseInsensitive) == 0)
        {
            if(branch_entrys.size())
            {
                BLOCK last_block = block_types.pop();
                if(last_block == BLOCK_IF)
                {
                    branches[current_line] = branch_entrys.pop();
                    branch_entrys.push(current_line);
                    block_types.push(BLOCK_ELSE);
                    continue;
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
                if(line[0].compare(keywords[i.end], Qt::CaseInsensitive) == 0)
                {
                    block_types.push(i.type);
                    branch_entrys.push(current_line);
                    break;
                }
                if(line[0].compare(keywords[i.begin], Qt::CaseInsensitive) == 0)
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
    }

    if(branch_entrys.size())
    {
        findAndThrowMissingBegin(0, block_types.pop());
        throw SteveInterpreterException("WTF #2", 0);
    }

    //Now parse a second time
    for(current_line = 0; current_line < code.size(); current_line++)
    {
        QStringList line = code[current_line].simplified().split(" ", QString::SkipEmptyParts);
        if(line.size() == 0)
            continue;

        for(auto i : blocks)
        {
            if(line[0].compare(keywords[i.begin], Qt::CaseInsensitive) == 0)
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
                        throw SteveInterpreterException(QObject::trUtf8("Zu viele Bezeichnungen."), current_line);

                    QString name = line[1].toLower();

                    QMap<QString, int> *customSymbols = i.type == BLOCK_NEW_COND ? &customConditions : &customInstructions;
                    if(customSymbols->contains(name))
                        throw SteveInterpreterException(QObject::trUtf8("%1 %2 existiert schon in Zeile %3").arg(keywords[i.begin]).arg(line[1]).arg((*customSymbols)[name]), current_line, line[1]);

                    (*customSymbols)[name] = current_line;
                }
                block_types.push(i.type);
                branch_entrys.push(current_line);
                break;
            }

            if(line[0].compare(keywords[i.end], Qt::CaseInsensitive) == 0)
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
    }

    if(branch_entrys.size())
        throw SteveInterpreterException("WTF #4", code.size() - 1);

    reset();
}

void SteveInterpreter::reset()
{
    current_line = 0;
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
    for(int line = 0; line < code.size(); line++)
    {
        if(line == current_line)
            std::cout << '>';
        else
            std::cout << ' ';

        std::cout << line << ": " << code[line].toStdString();
        if(branches.contains(line))
            std::cout << " (" << branches[line] << ")";

        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << QObject::trUtf8("Bedingungen: ").toStdString() << std::endl;
    if(customConditions.size() == 0)
        std::cout << QObject::trUtf8("(keine)").toStdString() << std::endl;
    else
        for(auto i : customConditions.keys())
            std::cout << QObject::trUtf8("%1 in Zeile %2").arg(i).arg(customConditions[i]).toStdString() << std::endl;

    std::cout << std::endl;

    std::cout << QObject::trUtf8("Anweisungen: ").toStdString() << std::endl;
    if(customInstructions.size() == 0)
        std::cout << QObject::trUtf8("(keine)").toStdString() << std::endl;
    else
        for(auto i : customInstructions.keys())
            std::cout << QObject::trUtf8("%1 in Zeile %2").arg(i).arg(customInstructions[i]).toStdString() << std::endl;
}

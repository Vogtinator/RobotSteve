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

void SteveInterpreter::setCode(QStringList code) throw (SteveInterpreterException)
{
    this->code = code;
    QStack<int> branch_entrys;
    QStack<BLOCK> block_types;
    current_line = code.size() - 1;

    while(current_line >= 0)
    {
        QString line = code[current_line].simplified().toLower();

        //Both special cases, BLOCK_ELSE has to be trated seperately
        if(line.startsWith(keywords[KEYWORD_IF]))
        {
            BLOCK last_block = block_types.pop();
            if(branch_entrys.size() && (last_block == BLOCK_IF || last_block == BLOCK_ELSE))
                branches[current_line] = branch_entrys.pop();
            else //TODO: Basierend auf block_types.pop sagen, was fehlt. Zeile: branches[branch_entrys.pop()]!
                throw QObject::trUtf8("Es fehlt ein %1.").arg(keywords[KEYWORD_IF_END]);
        }
        else if(line.startsWith(keywords[KEYWORD_ELSE]))
        {
            if(branch_entrys.size() && block_types.pop() == BLOCK_IF)
                branches[current_line] = branch_entrys.pop();
            else
                throw QObject::trUtf8("Es fehlt ein %1.").arg(keywords[KEYWORD_IF_END]);

            branch_entrys.push(current_line);
            block_types.push(BLOCK_ELSE);
        }
        else
        {
            for(auto i : blocks)
            {
                if(line.startsWith(keywords[i.end]))
                {
                    block_types.push(i.type);
                    branch_entrys.push(current_line);
                    break;
                }
                if(line.startsWith(keywords[i.begin]))
                {
                    if(branch_entrys.size())
                    {
                        BLOCK last_block = block_types.pop();
                        if(last_block == i.type)
                        {
                            branches[current_line] = branch_entrys.pop();
                            break;
                        }
                        else
                        {
                            for(auto block_keywords : blocks)
                            {
                                if(block_keywords.type == last_block)
                                    throw SteveInterpreterException(QObject::trUtf8("Es fehlt ein %1.").arg(keywords[block_keywords.begin]), current_line);
                            }
                            throw SteveInterpreterException("WTF #1", current_line);
                        }
                    }
                    throw SteveInterpreterException(QObject::trUtf8("Es fehlt ein %1.").arg(keywords[i.end]), current_line);
                }
            }
        }

        current_line--;
    }

    if(branch_entrys.size())
    {
        BLOCK i = block_types.pop();
        for(BlockKeywords block_keywords : blocks)
        {
            if(block_keywords.type == i)
                throw SteveInterpreterException(QObject::trUtf8("Es fehlt ein %1.").arg(keywords[block_keywords.begin]), 0);
        }
        throw SteveInterpreterException("WTF #2", current_line);
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
}

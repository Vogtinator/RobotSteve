#include <iostream>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QHash>
#include <QStack>
#include <QPixmap>
#include <QFontMetrics>
#include <QPainter>

#include "steveinterpreter.h"

#define DEFAULT_BLOCK_KEYWORDS(name) { .begin = KEYWORD_##name, \
        .end = KEYWORD_##name##_END, \
        .type = BLOCK_##name, \
    }

struct BlockKeywords {
    KEYWORD begin;
    KEYWORD end;
    BLOCK type;
};

static BlockKeywords blocks[] = {
    DEFAULT_BLOCK_KEYWORDS(IF),
    DEFAULT_BLOCK_KEYWORDS(REPEAT),
    DEFAULT_BLOCK_KEYWORDS(WHILE),
    DEFAULT_BLOCK_KEYWORDS(NEW_INSTR),
    DEFAULT_BLOCK_KEYWORDS(NEW_COND)
};

SteveInterpreter::SteveInterpreter(World *world) : world{world}
{
    keywords[KEYWORD_IF] = QObject::trUtf8("wenn");
    keywords[KEYWORD_NOT] = QObject::trUtf8("nicht");
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
    instructions[INSTR_QUIT] = QObject::trUtf8("beenden");
    instructions[INSTR_TRUE] = QObject::trUtf8("wahr");
    instructions[INSTR_FALSE] = QObject::trUtf8("falsch");
    instructions[INSTR_BREAK] = QObject::trUtf8("stop");

    conditions[COND_ALWAYS] = QObject::trUtf8("immer");
    conditions[COND_WALL] = QObject::trUtf8("wand");
    conditions[COND_CUBE] = QObject::trUtf8("würfel");
    conditions[COND_BRICK] = QObject::trUtf8("ziegel");
    conditions[COND_MARKED] = QObject::trUtf8("marke");
    conditions[COND_NORTH] = QObject::trUtf8("norden");
    conditions[COND_SOUTH] = QObject::trUtf8("süden");
    conditions[COND_EAST] = QObject::trUtf8("osten");
    conditions[COND_WEST] = QObject::trUtf8("westen");

    condition_functions[COND_ALWAYS] = SteveFunction(this, &SteveInterpreter::condAlways, false);
    condition_functions[COND_WALL] = SteveFunction(this, &SteveInterpreter::isWall, false);
    condition_functions[COND_CUBE] = SteveFunction(this, &SteveInterpreter::isCube, false);
    condition_functions[COND_BRICK] = SteveFunction(this, &SteveInterpreter::isBrick, true);
    condition_functions[COND_MARKED] = SteveFunction(this, &SteveInterpreter::isMarked, false);
    condition_functions[COND_NORTH] = SteveFunction(this, &SteveInterpreter::isNorth, false);
    condition_functions[COND_EAST] = SteveFunction(this, &SteveInterpreter::isEast, false);
    condition_functions[COND_SOUTH] = SteveFunction(this, &SteveInterpreter::isSouth, false);
    condition_functions[COND_WEST] = SteveFunction(this, &SteveInterpreter::isWest, false);

    instruction_functions[INSTR_STEP] = SteveFunction(this, &SteveInterpreter::step, true);
    instruction_functions[INSTR_TURNLEFT] = SteveFunction(this, &SteveInterpreter::turnLeft, true);
    instruction_functions[INSTR_TURNRIGHT] = SteveFunction(this, &SteveInterpreter::turnRight, true);
    instruction_functions[INSTR_PUTDOWN] = SteveFunction(this, &SteveInterpreter::deposit, true);
    instruction_functions[INSTR_PICKUP] = SteveFunction(this, &SteveInterpreter::pickup, true);
    instruction_functions[INSTR_MARK] = SteveFunction(this, &SteveInterpreter::mark, false);
    instruction_functions[INSTR_UNMARK] = SteveFunction(this, &SteveInterpreter::unmark, false);
    instruction_functions[INSTR_BREAK] = SteveFunction(this, &SteveInterpreter::breakpoint, false);
}

void SteveInterpreter::findAndThrowMissingBegin(int line, BLOCK block, const QString &affected) throw (SteveInterpreterException)
{
    for(auto block_keywords : blocks)
    {
        if(block_keywords.type == block || (block == BLOCK_ELSE && block_keywords.type == BLOCK_IF))
            throw SteveInterpreterException{QObject::trUtf8("Es fehlt ein %1.").arg(keywords[block_keywords.begin]), line, affected};
    }
}

void SteveInterpreter::setCode(QStringList code) throw (SteveInterpreterException)
{
    QStack<int> branch_entrys;
    QStack<BLOCK> block_types;
    bool in_custom_condition = false;

    code_valid = false;
    this->code = code;
    token.clear();
    branches.clear();
    custom_conditions.clear();
    custom_instructions.clear();

    current_line = code.size() - 1;
    for(current_line = code.size() - 1; current_line >= 0; current_line--)
    {
        token[current_line] = code[current_line].simplified().split(" ", QString::SkipEmptyParts);
        QStringList &line = token[current_line];
        if(line.size() == 0 || isComment(line[0]))
            continue;

        if(match(line[0], INSTR_FALSE) || match(line[0], INSTR_TRUE))
        {
            if(!in_custom_condition)
                throw SteveInterpreterException{QObject::trUtf8("%1 und %2 dürfen nur in einer eigenen Bedingung auftreten.").arg(str(INSTR_TRUE)).arg(str(INSTR_FALSE)), current_line};

            continue;
        }

        KEYWORD keyword = getKeyword(line[0]);
        if(keyword == -1)
            continue; //Not a keyword, ignore for now

        //Special case, KEYWORD_ELSE has to be trated seperately
        if(keyword == KEYWORD_ELSE)
        {
            if(!branch_entrys.size())
                throw SteveInterpreterException{QObject::trUtf8("Es fehlt ein %1.").arg(str(KEYWORD_IF_END)), current_line, str(KEYWORD_ELSE)};

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
                findAndThrowMissingBegin(current_line, last_block, str(KEYWORD_ELSE));
                throw SteveInterpreterException{"WTF #1", current_line};
            }
        }
        else
        {
            for(auto i : blocks)
            {
                if(keyword == i.end)
                {
                    if(keyword == KEYWORD_NEW_COND_END)
                                in_custom_condition = true;

                    block_types.push(i.type);
                    branch_entrys.push(current_line);
                    break;
                }
                else if(keyword == i.begin)
                {
                    if(!branch_entrys.size())
                        throw SteveInterpreterException{QObject::trUtf8("Es fehlt ein %1.").arg(str(i.end)), current_line, str(i.begin)};

                    if(keyword == KEYWORD_NEW_COND)
                                in_custom_condition = false;

                    BLOCK last_block = block_types.pop();
                    if(last_block == i.type || (last_block == BLOCK_ELSE && i.type == BLOCK_IF))
                    {
                        branches[current_line] = branch_entrys.pop();
                        break;
                    }
                    else
                    {
                        findAndThrowMissingBegin(current_line, last_block, str(i.begin));
                        throw SteveInterpreterException{"WTF #2", current_line};
                    }
                }
            }
        }
    }

    if(branch_entrys.size())
    {
        findAndThrowMissingBegin(0, block_types.pop());
        throw SteveInterpreterException("WTF #3", 0);
    }

    //Now parse a second time
    for(current_line = 0; current_line < code.size(); current_line++)
    {
        QStringList &line = token[current_line];
        if(line.size() == 0 || isComment(line[0]))
            continue;

        KEYWORD keyword = getKeyword(line[0]);
        if(keyword == -1)
            continue; //Not a keyword, ignore for now

        for(auto i : blocks)
        {
            if(keyword == i.begin)
            {
                if(i.type == BLOCK_NEW_COND || i.type == BLOCK_NEW_INSTR)
                {
                    if(block_types.size() > 0)
                    {
                        BLOCK in = block_types.pop();
                        for(auto bk : blocks)
                            if(bk.type == in)
                                throw SteveInterpreterException{QObject::trUtf8("%1 ist nicht in einem %2-Block erlaubt.").arg(str(i.begin)).arg(str(bk.begin)), current_line};

                        throw SteveInterpreterException{QObject::trUtf8("WTF #4"), current_line};
                    }

                    if(line.size() == 1)
                        throw SteveInterpreterException{QObject::trUtf8("Bezeichnung fehlt."), current_line};
                    else if(line.size() > 2)
                        throw SteveInterpreterException{QObject::trUtf8("Zu viele Bezeichnungen."), current_line};

                    QString name = line[1].toLower();
                    QRegExp validName("^(\\w|\\d)+$");

                    if(!validName.exactMatch(name))
                        throw SteveInterpreterException{QObject::trUtf8("Die Bezeichnung %1 enthält ungültige Zeichen.").arg(name), current_line, name};

                    if(getKeyword(name) != -1 || getInstruction(name) != -1 || getCondition(name) != -1)
                        throw SteveInterpreterException{QObject::trUtf8("Die Bezeichnung %1 ist ein reserviertes Wort.").arg(name), current_line, name};

                    auto &customSymbols = i.type == BLOCK_NEW_COND ? custom_conditions : custom_instructions;
                    if(customSymbols.contains(name))
                        throw SteveInterpreterException{QObject::trUtf8("%1 %2 existiert schon in Zeile %3").arg(str(i.begin)).arg(line[1]).arg(customSymbols[name]), current_line, line[1]};

                    customSymbols[name] = current_line;
                }
                block_types.push(i.type);
                branch_entrys.push(current_line);
                break; //Keyword found
            }
            else if(keyword == i.end)
            {

                if(branch_entrys.size())
                {
                    BLOCK last_block = block_types.pop();
                    if(last_block == i.type)
                    {
                        branches[current_line] = branch_entrys.pop();

                        if(keyword == KEYWORD_REPEAT_END)
                        {
                            if(line.size() != 1 && token[branches[current_line]].size() != 1)
                                throw SteveInterpreterException{QObject::trUtf8("Hier darf keine Bedingung sein, da in Zeile %1 eine angegeben wurde.").arg(branches[current_line]), current_line};

                            if(line.size() == 1 && token[branches[current_line]].size() == 1)
                                throw SteveInterpreterException{QObject::trUtf8("Hier wird eine Bedingung benötigt, da in Zeile %1 keine angegeben wurde.").arg(branches[current_line]), current_line};
                        }

                        break; //Keyword found
                    }
                }
                throw SteveInterpreterException{QObject::trUtf8("WTF #5"), current_line};
            }
        }
    }

    if(branch_entrys.size())
        throw SteveInterpreterException{"WTF #6", code.size() - 1};

    reset();

    code_valid = true;
}

void SteveInterpreter::reset()
{
    current_line = 0;
    stack.clear();
    loop_count.clear();
    custom_condition_return_stack.clear();
    coming_from_condition = coming_from_repeat_end = enter_sub = enter_else = execution_finished = hit_breakpoint = false;
}

bool SteveInterpreter::handleCondition(QString condition_str, bool &result) throw (SteveInterpreterException)
{
    QRegExp condition_regexp("^((\\w|\\d)+)(\\((\\d+)\\))?$");
    if(condition_regexp.indexIn(condition_str) == -1)
        throw SteveInterpreterException(QObject::trUtf8("Ungültige Bedingung."), current_line, condition_str);

    CONDITION condition = getCondition(condition_regexp.cap(1));
    if(condition != -1)
    {
        if(!condition_functions.contains(condition))
            throw SteveInterpreterException{QObject::trUtf8("WTF #7"), current_line};

        SteveFunction &func = condition_functions[condition];
        //Argument given
        if(!condition_regexp.cap(4).isEmpty())
        {
            if(!func.hasParam())
                throw SteveInterpreterException{QObject::trUtf8("Bedingung %1 kann mit einem Argument nichts anfangen.").arg(condition_regexp.cap(1)), current_line, condition_regexp.cap(3)};

            result = func(world, condition_regexp.cap(4).toInt());
        }
        else
            result = func(world);

        return true;
    }
    else if(custom_conditions.contains(condition_regexp.cap(1).toLower()))
    {
        stack.push(current_line);
        enter_sub = true;
        current_line = custom_conditions[condition_regexp.cap(1).toLower()];
        return false;
    }
    else
        throw SteveInterpreterException{QObject::trUtf8("Ich kenne die Bedingung %1 nicht.").arg(condition_regexp.cap(1)), current_line, condition_regexp.cap(1)};
}

bool SteveInterpreter::handleInstruction(QString instruction_str) throw (SteveInterpreterException)
{
    QRegExp instruction_regexp("^((\\w|\\d)+)(\\((\\d+)\\))?$");
    if(instruction_regexp.indexIn(instruction_str) == -1)
        throw SteveInterpreterException{QObject::trUtf8("Ungültige Anweisung."), current_line, instruction_str};

    INSTRUCTION instruction = getInstruction(instruction_regexp.cap(1));
    if(instruction != -1)
    {
        if(instruction == INSTR_TRUE)
        {
            custom_condition_return_stack.top() = true;
            return true;
        }
        else if(instruction == INSTR_FALSE)
        {
            custom_condition_return_stack.top() = false;
            return true;
        }
        else if(instruction == INSTR_QUIT)
        {
            execution_finished = true;
            return false;
        }

        if(!instruction_functions.contains(instruction))
            throw SteveInterpreterException{QObject::trUtf8("WTF #8"), current_line};

        SteveFunction &func = instruction_functions[instruction];
        //Argument given
        if(!instruction_regexp.cap(4).isEmpty())
        {
            if(!func.hasParam())
                throw SteveInterpreterException{QObject::trUtf8("Anweisung %1 kann mit einem Argument nichts anfangen!").arg(instruction_regexp.cap(1)), current_line, instruction_regexp.cap(3)};

            func(world, instruction_regexp.cap(4).toInt());
        }
        else
            func(world);

        return true;
    }
    else if(custom_instructions.contains(instruction_regexp.cap(1).toLower()))
    {
        stack.push(current_line);
        enter_sub = true;
        current_line = custom_instructions[instruction_regexp.cap(1).toLower()];
        return false;
    }
    else
        throw SteveInterpreterException{QObject::trUtf8("Ich kenne die Anweisung %1 nicht.").arg(instruction_regexp.cap(1)), current_line, instruction_regexp.cap(1)};
}

void SteveInterpreter::executeLine() throw (SteveInterpreterException)
{
    if(!code_valid)
        throw SteveInterpreterException{QObject::trUtf8("Der Code enthält Fehler."), 0};

    hit_breakpoint = false;

    if(current_line >= code.size())
    {
        execution_finished = true;
        return;
    }

    QStringList &line = token[current_line];
    if(line.size() == 0 || code[current_line].isEmpty() || isComment(line[0]))
    {
        current_line++;
        return;
    }

    //TODO: Backtrace?
#define MAX_STACK_SIZE 500000
    if(loop_count.size() > MAX_STACK_SIZE ||
            stack.size() > MAX_STACK_SIZE)
        throw SteveInterpreterException(QObject::trUtf8("Der Stack wird langsam ein bisschen zu groß.."), current_line);

    KEYWORD keyword = getKeyword(line[0]);

    if(keyword != -1)
    {
        if(keyword != KEYWORD_REPEAT)
            coming_from_repeat_end = false;
        if(keyword != KEYWORD_IF && keyword != KEYWORD_REPEAT && keyword != KEYWORD_WHILE)
            coming_from_condition = false;

        switch(keyword)
        {
        case KEYWORD_IF:
        {
            if(!(line.size() == 3 && match(line[2], KEYWORD_THEN)) && !(line.size() == 4 && match(line[1], KEYWORD_NOT) && match(line[3], KEYWORD_THEN)))
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1 [%2] [bedingung] %3").arg(str(KEYWORD_IF)).arg(str(KEYWORD_NOT)).arg(str(KEYWORD_THEN)), current_line);

            bool result;
            bool inverted = line.size() == 4;

            if(coming_from_condition)
            {
                result = custom_condition_return_stack.pop();
                coming_from_condition = false;
            }
            else
            {
                //If not handled, it means current_line is set to the beginning of a custom condition
                if(!handleCondition(inverted ? line[2] : line[1], result))
                    return;
            }

            if(inverted)
                result = !result;

            enter_else = !result;
            if(result)
                current_line++;
            else //Go to ELSE or IF_END
                current_line = branches[current_line];

            return;
        }
        case KEYWORD_ELSE:
            if(line.size() != 1)
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1").arg(str(KEYWORD_ELSE)), current_line);

            if(enter_else)
            {
                enter_else = false;
                current_line++;
            }
            else
            {
                //Go to IF_END
                current_line = branches[current_line];
            }
            return;
        case KEYWORD_IF_END:
            if(line.size() != 1)
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1").arg(str(KEYWORD_IF_END)), current_line);

            current_line++;
            return;

        case KEYWORD_REPEAT:
        {
            if(line.size() == 1)
            {
                current_line++;
                return;
            }

            bool repeat_always = line.size() == 2 && match(line[1], COND_ALWAYS);
            bool repeat_count = line.size() == 3 && match(line[2], KEYWORD_TIMES);
            bool repeat_condition = (line.size() == 3 && match(line[1], KEYWORD_WHILE)) || (line.size() == 4 && match(line[1], KEYWORD_WHILE) && match(line[2], KEYWORD_NOT));
            bool inverted = line.size() == 4;

            //None of the above
            if(!(repeat_always || repeat_count || repeat_condition))
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1\n%1 [zahl] %2\n%1 %3 [%4] [bedingung]\n%1 %5").arg(str(KEYWORD_REPEAT)).arg(str(KEYWORD_TIMES))
                                                .arg(str(KEYWORD_WHILE)).arg(str(KEYWORD_NOT)).arg(str(COND_ALWAYS)), current_line);

            if(repeat_always)
                current_line++;

            else if(repeat_count)
            {
                if(coming_from_repeat_end)
                {
                    loop_count.top()--;
                    coming_from_repeat_end = false;
                    int count = loop_count.top();
                    if(count <= 0)
                    {
                        current_line = branches[current_line] + 1;
                        loop_count.pop();
                    }
                    else
                        current_line++;

                    return;
                }

                bool is_numeric;
                int count = line[1].toInt(&is_numeric);
                if(!is_numeric)
                    throw SteveInterpreterException(QObject::trUtf8("%1 ist keine Zahl.").arg(line[1]), current_line, line[1]);

                if(count < 0 || count > 9999)
                    throw SteveInterpreterException(QObject::trUtf8("Die Zahl muss >= 0 und kleiner als 10000 sein."), current_line, line[1]);
                else if(count == 0)
                {
                    current_line = branches[current_line] + 1;
                    return;
                }

                loop_count.push(count);
                current_line++;
            }
            else
            {
                bool result;
                if(coming_from_condition)
                {
                    result = custom_condition_return_stack.pop();
                    coming_from_condition = false;
                }
                else
                {
                    //If not handled, it means current_line is set to the beginning of a custom condition
                    if(!handleCondition(inverted ? line[3] : line[2], result))
                        return;
                }

                if(result != inverted)
                    current_line++;
                else
                    current_line = branches[current_line] + 1;
            }
            return;
        }
        case KEYWORD_REPEAT_END:
        {
            //Condition here instead of at the beginning: DO...WHILE
            if(line.size() == 3 || line.size() == 4)
            {
                if(!match(line[1], KEYWORD_WHILE) || (line.size() == 4 && !match(line[2], KEYWORD_NOT)))
                    throw SteveInterpreterException(QObject::trUtf8("Syntax: %1 %2 [%3] [bedingung]").arg(str(KEYWORD_REPEAT_END)).arg(str(KEYWORD_WHILE)).arg(str(KEYWORD_NOT)), current_line);

                bool inverted = line.size() == 4;

                bool result;
                if(coming_from_condition)
                    result = custom_condition_return_stack.pop();
                else
                {
                    //If not handled, it means current_line is set to the beginning of a custom condition
                    if(!handleCondition(inverted ? line[3] : line[2], result))
                        return;
                }

                if(result != inverted)
                    current_line = branches[current_line];
                else
                    current_line++;

                return;
            }
            //No condition here: WHILE...DO
            else if(line.size() == 1)
            {
                current_line = branches[current_line];
                coming_from_repeat_end = true;
                return;
            }
            else
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1\n%1 %2 [%3] [bedingung]").arg(str(KEYWORD_REPEAT_END)).arg(str(KEYWORD_WHILE)).arg(str(KEYWORD_NOT)), current_line);

            return;
        }

        case KEYWORD_WHILE:
        {
            if(!(line.size() == 3 && match(line[2], KEYWORD_DO)) &&
                    !(line.size() == 4 && match(line[1], KEYWORD_NOT) && match(line[3], KEYWORD_DO)))
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1 [%2] [bedingung] %3").arg(str(KEYWORD_WHILE)).arg(str(KEYWORD_NOT)).arg(str(KEYWORD_DO)), current_line);

            bool inverted = line.size() == 4;

            bool result;
            if(coming_from_condition)
            {
                result = custom_condition_return_stack.pop();
                coming_from_condition = false;
            }
            else
            {
                if(!handleCondition(inverted ? line[2] : line[1], result))
                    return;
            }

            if(result != inverted)
                current_line++;
            else
                current_line = branches[current_line] + 1;

            return;
        }
        case KEYWORD_WHILE_END:
            if(line.size() != 1)
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1").arg(str(KEYWORD_WHILE_END)), current_line);

            current_line = branches[current_line];
            return;

        case KEYWORD_NEW_INSTR:
        case KEYWORD_NEW_COND:
            //True is default
            custom_condition_return_stack.push(true);

            if(enter_sub)
            {
                enter_sub = false;
                current_line++;
            }
            else
                current_line = branches[current_line] + 1;

            return;
        case KEYWORD_NEW_INSTR_END:
            current_line = stack.pop() + 1;
            return;
        case KEYWORD_NEW_COND_END:
            coming_from_condition = true;
            current_line = stack.pop();
            if(custom_condition_return_stack.size() == 0)
                throw SteveInterpreterException(QObject::trUtf8("WTF #9"), current_line);

            return;

        default:
            throw SteveInterpreterException(QObject::trUtf8("%1 macht hier keinen Sinn.").arg(line[0]), current_line);
        }
    }

    if(line.size() != 1)
        throw SteveInterpreterException(QObject::trUtf8("Ungültige Anweisung."), current_line);

    if(handleInstruction(line[0]))
        current_line++;

    return;

    //Never called, handleInstruction throws it for us.
    throw SteveInterpreterException(QObject::trUtf8("Ich hab` keine Ahnung, was %1 bedeutet :-(").arg(code[current_line]), current_line);
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
    if(custom_conditions.size() == 0)
        std::cout << QObject::trUtf8("(keine)").toStdString() << std::endl;
    else
        for(auto i : custom_conditions.keys())
            std::cout << QObject::trUtf8("%1 in Zeile %2").arg(i).arg(custom_conditions[i]).toStdString() << std::endl;

    std::cout << std::endl;

    std::cout << QObject::trUtf8("Anweisungen: ").toStdString() << std::endl;
    if(custom_instructions.size() == 0)
        std::cout << QObject::trUtf8("(keine)").toStdString() << std::endl;
    else
        for(auto i : custom_instructions.keys())
            std::cout << QObject::trUtf8("%1 in Zeile %2").arg(i).arg(custom_instructions[i]).toStdString() << std::endl;

    std::cout << QObject::trUtf8("Status: ").toStdString() << std::endl;
    if(enter_else)
        std::cout << QObject::trUtf8("Ich würde den nächsten %1-Block betreten.").arg(str(KEYWORD_ELSE)).toStdString() << std::endl;
    if(enter_sub)
        std::cout << QObject::trUtf8("Ich würde den nächsten %1- und %2-Block betreten.").arg(str(KEYWORD_NEW_INSTR)).arg(str(KEYWORD_NEW_COND)).toStdString() << std::endl;
    if(coming_from_repeat_end)
        std::cout << QObject::trUtf8("Ich komme gerade von %1.").arg(str(KEYWORD_REPEAT_END)).toStdString() << std::endl;
    if(coming_from_condition)
        std::cout << QObject::trUtf8("Ich komme gerade von einer selbstdefinierten Bedingung. Der Wert ist %1").arg(custom_condition_return_stack.top() ? "WAHR" : "FALSCH" ).toStdString() << std::endl;
    if(executionFinished())
        std::cout << QObject::trUtf8("Das Programm ist zuende.").toStdString() << std::endl;
}

//Conditions
bool SteveInterpreter::condAlways(World *world, bool has_param, int param)
{
    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return true;
}

bool SteveInterpreter::isWall(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->frontBlocked();
}

bool SteveInterpreter::isCube(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->isCube();
}

bool SteveInterpreter::isBrick(World *world, bool has_param, int param)
{
    if(!has_param)
        return world->getStackSize() > 0;

    return world->getStackSize() == param;
}

bool SteveInterpreter::isMarked(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->isMarked();
}

bool SteveInterpreter::isNorth(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->getOrientation() == ORIENT_NORTH;
}

bool SteveInterpreter::isEast(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->getOrientation() == ORIENT_EAST;
}

bool SteveInterpreter::isSouth(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->getOrientation() == ORIENT_SOUTH;
}

bool SteveInterpreter::isWest(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->getOrientation() == ORIENT_WEST;
}

//Instructions
bool SteveInterpreter::unmark(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    world->setMark(false);
    return true;
}

bool SteveInterpreter::mark(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    world->setMark(true);
    return true;
}

bool SteveInterpreter::pickup(World *world, bool has_param, int param)
{
    if(!has_param)
        param = 1;

    if(world->isWall())
        throw SteveInterpreterException(QObject::trUtf8("Steve steht vor einer Wand und weiß nicht, was er jetzt tun soll."), current_line);

    if(!world->pickup(param))
        throw SteveInterpreterException(QObject::trUtf8("Steve sieht nicht genug Ziegel zum Aufheben."), current_line);

    return true;
}

bool SteveInterpreter::deposit(World *world, bool has_param, int param)
{
    if(!has_param)
        param = 1;

    if(world->isWall())
        throw SteveInterpreterException(QObject::trUtf8("Steve steht vor einer Wand und weiß nicht, was er jetzt tun soll."), current_line);

    if(!world->deposit(param))
        throw SteveInterpreterException(QObject::trUtf8("Maximale Höhe erreicht. Steve kann nicht höher heben."), current_line);

    return true;
}

bool SteveInterpreter::turnRight(World *world, bool has_param, int param)
{
    if(!has_param)
        param = 1;

    world->turnRight(param);
    return true;
}

bool SteveInterpreter::turnLeft(World *world, bool has_param, int param)
{
    if(!has_param)
        param = 1;

    world->turnLeft(param);
    return true;
}

bool SteveInterpreter::step(World *world, bool has_param, int param)
{
    if(!has_param)
        param = 1;

    while(param--)
        if(!world->stepForward())
            throw SteveInterpreterException(QObject::trUtf8("Steve war so dumm und ist gegen die Wand gelaufen!"), current_line);

    return true;
}

bool SteveInterpreter::breakpoint(World *world, bool has_param, int param)
{
    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    hit_breakpoint = true;
    return true;
}

//Other private functions
KEYWORD SteveInterpreter::getKeyword(QString string)
{
    for(auto i : keywords.keys())
    {
        if(match(string, i))
            return i;
    }

    return static_cast<KEYWORD>(-1);
}

INSTRUCTION SteveInterpreter::getInstruction(QString string)
{
    for(auto i : instructions.keys())
    {
        if(match(string, i))
            return i;
    }

    return static_cast<INSTRUCTION>(-1);
}

CONDITION SteveInterpreter::getCondition(QString string)
{
    for(auto i : conditions.keys())
    {
        if(match(string, i))
            return i;
    }

    return static_cast<CONDITION>(-1);
}

bool SteveInterpreter::isComment(const QString &s)
{
    return s.startsWith(";") || s.startsWith("#") || s.startsWith("//");
}

template <typename TOKEN>
bool SteveInterpreter::match(const QString &str, const TOKEN tok)
{
    return str.compare(this->str(tok), Qt::CaseInsensitive) == 0;
}

const QString &SteveInterpreter::str(KEYWORD keyword)
{
    return keywords[keyword];
}

const QString &SteveInterpreter::str(INSTRUCTION instr)
{
    return instructions[instr];
}

const QString &SteveInterpreter::str(CONDITION cond)
{
    return conditions[cond];
}

//Structure chart generation

struct StructureBlock {
    QString title;
    QList<int> code_lines;
    QStringList code;
    QPixmap chart;
};

void SteveInterpreter::drawText(int x, int y, QPainter &painter, const QString &text)
{
    QStringList token = text.split(" ", QString::SkipEmptyParts);
    QPen saved_pen = painter.pen();

    static QFont normal{"Monospace"};
    normal.setStyleHint(QFont::Monospace);
    normal.setPointSize(9);

    static QFont bold{normal};
    bold.setWeight(QFont::Bold);

    static QFontMetrics metrics_normal{normal};
    static QFontMetrics metrics_bold{bold};

    for(const QString &tok : token)
    {
        QFontMetrics *metrics = &metrics_bold;
        painter.setFont(bold);
        if(getKeyword(tok) != -1)
            painter.setPen(QColor(0, 128, 0));
        else if(getCondition(tok) != -1 || custom_conditions.contains(tok.toLower()))
            painter.setPen(QColor(192,16, 112));
        else if(getInstruction(tok) != -1 || custom_instructions.contains(tok.toLower()))
            painter.setPen(QColor(128, 0, 0));
        else
        {
            metrics = &metrics_normal;
            painter.setFont(normal);
            painter.setPen(QColor(0, 0, 0));
        }

        painter.drawText(x, y + metrics->ascent() + 2, tok);
        x += metrics->width(tok) + metrics->width(' ');
    }

    painter.setPen(saved_pen);
}

int SteveInterpreter::textWidth(const QString &text)
{
    int width = 0;

    QStringList token = text.split(" ", QString::SkipEmptyParts);

    static QFont normal{"Monospace"};
    normal.setStyleHint(QFont::Monospace);
    normal.setPointSize(9);

    static QFont bold{normal};
    bold.setWeight(QFont::Bold);

    static QFontMetrics metrics_normal{normal};
    static QFontMetrics metrics_bold{bold};

    for(const QString &tok : token)
    {
        QFontMetrics *metrics = &metrics_bold;
        if(getKeyword(tok) == -1 && getCondition(tok) == -1 && getInstruction(tok) == -1 && !custom_conditions.contains(tok.toLower()) && !custom_instructions.contains(tok.toLower()))
            metrics = &metrics_normal;

        width += metrics->width(tok) + metrics->width(' ');
    }
    return width;
}

int SteveInterpreter::textHeight()
{
    static QFont normal{"Monospace"};
    normal.setStyleHint(QFont::Monospace);
    normal.setPointSize(9);

    static QFontMetrics metrics_normal{normal};
    return metrics_normal.height() + 2;
}

QPixmap SteveInterpreter::structureChart() throw (SteveInterpreterException)
{
    if(!code_valid)
        throw SteveInterpreterException(QObject::trUtf8("Der Code enthält Fehler."), 0);

    int line;

    QVector<StructureBlock> blocks;
    QVector<QPixmap> pixmaps;
    blocks.resize(1 + custom_instructions.size() + custom_conditions.size());

    blocks[0].title = QObject::trUtf8("Hauptprogramm");

    //Main block
    int current_block = 0, last_block = 0;
    for(line = 0; line < code.size(); line++)
    {
        if(token[line].size() < 1)
            continue;

        KEYWORD keyword = getKeyword(token[line][0]);
        switch(keyword)
        {
        case KEYWORD_NEW_COND_END:
        case KEYWORD_NEW_INSTR_END:
            current_block = 0;
            break;
        case KEYWORD_NEW_COND:
            current_block = ++last_block;
            blocks[current_block].title = QObject::trUtf8("Bedingung %0").arg(custom_conditions.key(line, "WTF #10"));
            break;
        case KEYWORD_NEW_INSTR:
            current_block = ++last_block;
            blocks[current_block].title = QObject::trUtf8("Anweisung %0").arg(custom_instructions.key(line, "WTF #11"));
            break;
        default:
            blocks[current_block].code.append(code[line].trimmed());
            blocks[current_block].code_lines.append(line);
        }
    }

    for(StructureBlock &block : blocks)
        pixmaps.append(structureChartBlock(block));

    //Calculate dimensions of pixmap
    QFont title_font{"Monospace"};
    title_font.setStyleHint(QFont::Monospace);
    title_font.setWeight(QFont::Bold);
    QFontMetrics metrics{title_font};

    int height = 0;
    int width = 0;
    for(int i = 0; i < blocks.size(); i++)
    {
        if(pixmaps[i].height() > height)
            height = pixmaps[i].height();

        if(pixmaps[i].width() < metrics.width(blocks[i].title))
            width += metrics.width(blocks[i].title);
        else
            width += pixmaps[i].width();

        width += 10;
    }

    height += metrics.height() * 2;

    QPixmap pixmap{width, height};
    pixmap.fill(Qt::white);
    QPainter painter{&pixmap};
    painter.setFont(title_font);

    int x = 0;
    for(int i = 0; i < blocks.size(); i++)
    {
        painter.drawText(x, metrics.ascent(), blocks[i].title);
        painter.drawPixmap(x, metrics.height() * 2, pixmaps[i]);

        if(metrics.width(blocks[i].title) > pixmaps[i].width())
            x += metrics.width(blocks[i].title);
        else
            x += pixmaps[i].width();

        x += 10;
    }

    return pixmap;
}

QPixmap SteveInterpreter::structureChartBlock(const StructureBlock &sb)
{
    if(sb.code.size() == 0)
        return {};

    QHash<int,QPixmap> child_pixmaps;
    QHash<int,StructureBlock> child_blocks;

    int width = 0, height = 0;

    //Calculate height and get QPixmaps for child blocks
    for(int line = 0; line < sb.code.size(); line++)
    {
        int actual_line = sb.code_lines[line];
        if(token[actual_line].size() < 1)
            continue;

        KEYWORD keyword = getKeyword(token[actual_line][0]);
        StructureBlock child_block;

        //TODO: This works, but is ugly code
        if(keyword == KEYWORD_IF)
        {
            int child_start = line;
            int actual_if_start = actual_line;
            while(actual_line < branches[actual_if_start])
            {
                child_block.code.append(sb.code[line]);
                child_block.code_lines.append(sb.code_lines[line]);
                actual_line = sb.code_lines[++line];
            }
            if(match(token[actual_line][0], KEYWORD_ELSE))
            {
                actual_if_start = actual_line;
                while(actual_line < branches[actual_if_start])
                {
                    child_block.code.append(sb.code[line]);
                    child_block.code_lines.append(sb.code_lines[line]);
                    actual_line = sb.code_lines[++line];
                }
            }
            child_block.code.append(sb.code[line]);
            child_block.code_lines.append(sb.code_lines[line]);

            child_blocks[child_start] = child_block;
            child_pixmaps[child_start] = structureChartIfBlock(child_block);

            height += child_pixmaps[child_start].height();
            if(child_pixmaps[child_start].width() > width)
                width = child_pixmaps[child_start].width();
        }
        else if(keyword == KEYWORD_REPEAT || keyword == KEYWORD_WHILE)
        {
            int child_start = line;
            int actual_block_start = actual_line;
            while(actual_line < branches[actual_block_start])
            {
                child_block.code.append(sb.code[line]);
                child_block.code_lines.append(sb.code_lines[line]);
                actual_line = sb.code_lines[++line];
            }
            child_block.code.append(sb.code[line]);
            child_block.code_lines.append(sb.code_lines[line]);

            child_blocks[child_start] = child_block;
            child_pixmaps[child_start] = structureChartOtherBlock(child_block);
            height += child_pixmaps[child_start].height();
            int w = child_pixmaps[child_start].width();
            if(w > width)
                width = w;
        }
        else //Normal text
        {
            height += textHeight();
            int w = textWidth(sb.code[line]);
            if(w > width)
                width = w;
        }
    }

    QPixmap pixmap{width + 4, height + 4};
    pixmap.fill(Qt::white);
    QPainter painter{&pixmap};

    //Finally, render!
    int current_y = 2;
    for(int line = 0; line < sb.code.size(); line++)
    {
        int actual_line = sb.code_lines[line];
        if(token[actual_line].size() < 1)
            continue;

        KEYWORD keyword = getKeyword(token[actual_line][0]);
        if(keyword == KEYWORD_IF || keyword == KEYWORD_REPEAT || keyword == KEYWORD_WHILE)
        {
            painter.drawPixmap(2, current_y, child_pixmaps[line]);
            current_y += child_pixmaps[line].height();
            line += child_blocks[line].code.size() - 1;
        }
        else
        {
            drawText(2, current_y, painter, sb.code[line]);
            current_y += textHeight();
        }
    }

    painter.drawRect(0, 0, width + 2, height + 2);

    return pixmap;
}

QPixmap SteveInterpreter::structureChartIfBlock(const StructureBlock &sb)
{
    Q_ASSERT(match(token[sb.code_lines[0]][0], KEYWORD_IF));

    StructureBlock if_true_block, if_false_block;

    int line = 0, actual_if_line = sb.code_lines[line];
    int actual_line = sb.code_lines[++line];
    while(actual_line < branches[actual_if_line])
    {
        if_true_block.code_lines.append(actual_line);
        if_true_block.code.append(sb.code[line]);
        actual_line = sb.code_lines[++line];
    }
    if(match(token[actual_line][0], KEYWORD_ELSE))
    {
        //This line with "else" as new start
        actual_if_line = actual_line;

        line += 1;
        actual_line = sb.code_lines[line];
        while(actual_line < branches[actual_if_line])
        {
            if_false_block.code_lines.append(actual_line);
            if_false_block.code.append(sb.code[line]);
            actual_line = sb.code_lines[++line];
        }
    }

    QPixmap if_true = structureChartBlock(if_true_block);
    QPixmap if_false = structureChartBlock(if_false_block);

    int width = if_true.width() + if_false.width();
    int height = std::max(if_true.height(), if_false.height());

    int header_height = 2*textHeight();
    int header_text_width = textWidth(sb.code[0]);
    if(width < header_text_width)
        width = header_text_width;

    QPixmap pixmap{width, height + header_height};
    pixmap.fill(Qt::white);
    QPainter painter{&pixmap};

    drawText((width/2)-(header_text_width/2) + 2, 0, painter, sb.code[0]);
    painter.drawLine(0, header_height, width, header_height);

    if(if_true.width() > 0 && if_false.width() > 0)
    {
        painter.drawLine(1, textHeight(), if_true.width(), header_height - 2);
        painter.drawLine(width, textHeight(), if_true.width(), header_height - 2);
    }
    else if(if_true.width() > 0)
        painter.drawLine(1, textHeight(), width - 2, header_height - 2);
    else
        painter.drawLine(width, textHeight(), 2, header_height - 2);

    if(if_true.width() > 0)
        drawText(2, header_height - textHeight(), painter, QObject::trUtf8("W"));
    if(if_false.width() > 0)
        drawText(width - textWidth(QObject::trUtf8("F")) - 2, header_height - textHeight(), painter, QObject::trUtf8("F"));

    painter.drawPixmap(0, header_height, if_true);
    painter.drawPixmap(width - if_false.width() + 1, header_height, if_false);

    painter.drawRect(0, 0, pixmap.width() - 1, pixmap.height() - 2);

    return pixmap;
}

QPixmap SteveInterpreter::structureChartOtherBlock(const StructureBlock &sb)
{
    Q_ASSERT(match(token[sb.code_lines[0]][0], KEYWORD_REPEAT) || match(token[sb.code_lines[0]][0], KEYWORD_WHILE));

    StructureBlock this_block;

    int line = 0, actual_start_line = sb.code_lines[line];
    int actual_line = sb.code_lines[++line];
    while(actual_line < branches[actual_start_line])
    {
        this_block.code_lines.append(actual_line);
        this_block.code.append(sb.code[line]);
        actual_line = sb.code_lines[++line];
    }

    QPixmap block_pixmap = structureChartBlock(this_block);

    int width = block_pixmap.width();
    int height = block_pixmap.height();

    int intendation_width = 15;

    if(width < textWidth(sb.code[0]))
        width = textWidth(sb.code[0]);
    else if(width < textWidth(sb.code.last()))
        width = textWidth(sb.code.last());

    QPixmap pixmap{width + intendation_width, height + 2*textHeight()};
    pixmap.fill(Qt::white);
    QPainter painter{&pixmap};

    drawText(2, 0, painter, sb.code[0]);

    //Strip the right border
    painter.drawPixmap(intendation_width, textHeight(), block_pixmap.width() - 2, block_pixmap.height(),
                       block_pixmap, 0, 0, block_pixmap.width() - 2, block_pixmap.height());
    painter.drawRect(intendation_width, textHeight(), width - 2, block_pixmap.height() - 2);

    painter.drawRect(0, 0, pixmap.width() - 2, pixmap.height() - 2);

    drawText(2, pixmap.height() - textHeight(), painter, sb.code.last());

    return pixmap;
}

//SteveInterpreterException
const char* SteveInterpreterException::what()
{
    return QObject::trUtf8("Fehler in Zeile %1:\n%2").arg(line).arg(error).toUtf8().data();
}

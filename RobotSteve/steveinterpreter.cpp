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

SteveInterpreter::SteveInterpreter(World *world) : world(world)
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
    instructions[INSTR_TRUE] = QObject::trUtf8("wahr");
    instructions[INSTR_FALSE] = QObject::trUtf8("falsch");

    conditions[COND_ALWAYS] = QObject::trUtf8("immer");
    conditions[COND_NEVER] = QObject::trUtf8("nie");
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

    condition_functions[COND_ALWAYS] = SteveFunction(this, &SteveInterpreter::cond_always, false);
    condition_functions[COND_NEVER] = SteveFunction(this, &SteveInterpreter::cond_always, false, true);
    condition_functions[COND_ISWALL] = SteveFunction(this, &SteveInterpreter::is_wall, false);
    condition_functions[COND_NOTISWALL] = SteveFunction(this, &SteveInterpreter::is_wall, false, true);
    condition_functions[COND_ISBRICK] = SteveFunction(this, &SteveInterpreter::is_brick, true);
    condition_functions[COND_NOTISBRICK] = SteveFunction(this, &SteveInterpreter::is_brick, true, true);
    condition_functions[COND_MARKED] = SteveFunction(this, &SteveInterpreter::is_marked, false);
    condition_functions[COND_NOTMARKED] = SteveFunction(this, &SteveInterpreter::is_marked, false, true);
    condition_functions[COND_ISNORTH] = SteveFunction(this, &SteveInterpreter::is_north, false);
    condition_functions[COND_ISEAST] = SteveFunction(this, &SteveInterpreter::is_east, false);
    condition_functions[COND_ISSOUTH] = SteveFunction(this, &SteveInterpreter::is_south, false);
    condition_functions[COND_ISWEST] = SteveFunction(this, &SteveInterpreter::is_west, false);
    condition_functions[COND_ISFULL] = SteveFunction(this, &SteveInterpreter::is_full, false);
    condition_functions[COND_NOTISFULL] = SteveFunction(this, &SteveInterpreter::is_full, false, true);
    condition_functions[COND_ISEMPTY] = SteveFunction(this, &SteveInterpreter::is_empty, false);
    condition_functions[COND_NOTISEMPTY] = SteveFunction(this, &SteveInterpreter::is_empty, false, true);
    condition_functions[COND_HASBRICK] = SteveFunction(this, &SteveInterpreter::has_bricks, true);

    instruction_functions[INSTR_STEP] = SteveFunction(this, &SteveInterpreter::step, true);
    instruction_functions[INSTR_TURNLEFT] = SteveFunction(this, &SteveInterpreter::turnLeft, false);
    instruction_functions[INSTR_TURNRIGHT] = SteveFunction(this, &SteveInterpreter::turnRight, false);
    instruction_functions[INSTR_PUTDOWN] = SteveFunction(this, &SteveInterpreter::putdown, true);
    instruction_functions[INSTR_PICKUP] = SteveFunction(this, &SteveInterpreter::pickup, true);
    instruction_functions[INSTR_MARK] = SteveFunction(this, &SteveInterpreter::mark, false);
    instruction_functions[INSTR_UNMARK] = SteveFunction(this, &SteveInterpreter::unmark, false);
    instruction_functions[INSTR_WAIT] = SteveFunction(this, &SteveInterpreter::wait, true);
    instruction_functions[INSTR_TONE] = SteveFunction(this, &SteveInterpreter::tone, false);
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
        if(line.size() == 0)
            continue;

        if(line[0].compare(instructions[INSTR_FALSE], Qt::CaseInsensitive) == 0 || line[0].compare(instructions[INSTR_TRUE], Qt::CaseInsensitive) == 0)
        {
            if(!branch_entrys.size() || block_types.top() != BLOCK_NEW_COND)
                throw SteveInterpreterException(QObject::trUtf8("%1 und %2 dürfen nur in einer eigenen Bedingung auftreten.").arg(instructions[INSTR_TRUE]).arg(instructions[INSTR_FALSE]), current_line);

            branches[current_line] = branch_entrys.top();

            continue;
        }

        KEYWORD keyword = getKeyword(line[0]);
        if(keyword == -1)
            continue; //Not a keyword, ignore for now

        //Special case, KEYWORD_ELSE has to be trated seperately
        if(keyword == KEYWORD_ELSE)
        {
            if(!branch_entrys.size())
                throw SteveInterpreterException(QObject::trUtf8("Es fehlt ein %1.").arg(keywords[KEYWORD_IF_END]), current_line, keywords[KEYWORD_ELSE]);

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
        else
        {
            for(auto i : blocks)
            {
                if(keyword == i.end)
                {
                    block_types.push(i.type);
                    branch_entrys.push(current_line);
                    break;
                }
                else if(keyword == i.begin)
                {
                    if(!branch_entrys.size())
                        throw SteveInterpreterException(QObject::trUtf8("Es fehlt ein %1.").arg(keywords[i.end]), current_line, keywords[i.begin]);

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
        QStringList &line = token[current_line];
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

                    if(getKeyword(name) != -1 || getInstruction(name) != -1 || getCondition(name) != -1)
                        throw SteveInterpreterException(QObject::trUtf8("Die Bezeichnung %1 ist ein reserviertes Wort.").arg(name), current_line, name);

                    QMap<QString, int> *customSymbols = i.type == BLOCK_NEW_COND ? &custom_conditions : &custom_instructions;
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

    code_valid = true;
}

void SteveInterpreter::reset()
{
    current_line = 0;
    stack.clear();
    coming_from_condition = coming_from_repeat_end = enter_sub = enter_else = execution_finished = false;
    condition_exit = true;
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
            throw SteveInterpreterException(QObject::trUtf8("WTF #6"), current_line);

        SteveFunction &func = condition_functions[condition];
        //Argument given
        if(!condition_regexp.cap(4).isEmpty())
        {
            if(!func.hasParam())
                throw SteveInterpreterException(QObject::trUtf8("Bedingung %1 kann mit einem Argument nichts anfangen.").arg(condition_regexp.cap(1)), current_line, condition_regexp.cap(3));

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
        throw SteveInterpreterException(QObject::trUtf8("Ich kenne die Bedingung %1 nicht.").arg(condition_regexp.cap(1)), current_line, condition_regexp.cap(1));
}

bool SteveInterpreter::handleInstruction(QString instruction_str) throw (SteveInterpreterException)
{
    QRegExp instruction_regexp("^((\\w|\\d)+)(\\((\\d+)\\))?$");
    if(instruction_regexp.indexIn(instruction_str) == -1)
        throw SteveInterpreterException(QObject::trUtf8("Ungültige Anweisung."), current_line, instruction_str);

    INSTRUCTION instruction = getInstruction(instruction_regexp.cap(1));
    if(instruction != -1)
    {
        if(instruction == INSTR_TRUE)
        {
            condition_exit = true;
            current_line = branches[current_line];
            return false;
        }
        else if(instruction == INSTR_FALSE)
        {
            condition_exit = false;
            current_line = branches[current_line];
            return false;
        }
        else if(instruction == INSTR_QUIT)
        {
            execution_finished = true;
            return false;
        }

        if(!instruction_functions.contains(instruction))
            throw SteveInterpreterException(QObject::trUtf8("WTF #7"), current_line);

        SteveFunction &func = instruction_functions[instruction];
        //Argument given
        if(!instruction_regexp.cap(4).isEmpty())
        {
            if(!func.hasParam())
                throw SteveInterpreterException(QObject::trUtf8("Anweisung %1 kann mit einem Argument nichts anfangen!").arg(instruction_regexp.cap(1)), current_line, instruction_regexp.cap(3));

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
        throw SteveInterpreterException(QObject::trUtf8("Ich kenne die Anweisung %1 nicht.").arg(instruction_regexp.cap(1)), current_line, instruction_regexp.cap(1));
}

void SteveInterpreter::executeLine() throw (SteveInterpreterException)
{
    if(!code_valid)
        throw SteveInterpreterException(QObject::trUtf8("Der Code ist nicht richtig."), 0, code.size() - 1);

    if(current_line >= code.size())
    {
        execution_finished = true;
        return;
    }

    QStringList &line = token[current_line];
    if(line.size() == 0 || code[current_line].isEmpty())
    {
        current_line++;
        return;
    }

#define MAX_STACK_SIZE 500000
    if(repeat_needs_condition.size() > MAX_STACK_SIZE ||
            loop_count.size() > MAX_STACK_SIZE ||
            stack.size() > MAX_STACK_SIZE)
        throw SteveInterpreterException(QObject::trUtf8("Der Stack wird langsam ein bisschen zu groß.."), current_line);

    KEYWORD keyword = getKeyword(line[0]);
    if(keyword != -1)
    {
        switch(keyword)
        {
        case KEYWORD_IF:
        {
            if(line.size() != 3 || getKeyword(line[2]) != KEYWORD_THEN)
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1 [bedingung] %2").arg(keywords[KEYWORD_IF]).arg(keywords[KEYWORD_THEN]), current_line);

            bool result;
            if(coming_from_condition)
            {
                result = condition_exit;
                coming_from_condition = false;
            }
            else
            {
                //If not handled, it means current_line is set to the beginning of a custom condition
                if(!handleCondition(line[1], result))
                    return;
            }

            enter_else = !result;
            if(result)
                current_line++;
            else //Go to ELSE or IF_END
                current_line = branches[current_line];

            return;
        }
        case KEYWORD_ELSE:
            if(line.size() != 1)
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1").arg(keywords[KEYWORD_ELSE]), current_line);

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
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1").arg(keywords[KEYWORD_IF_END]), current_line);

            current_line++;
            return;

        case KEYWORD_REPEAT:
        {
            if(line.size() == 1)
            {
                repeat_needs_condition.push(true);
                current_line++;
                return;
            }
            repeat_needs_condition.push(false);

            KEYWORD kwhile = static_cast<KEYWORD>(-1), ktimes = static_cast<KEYWORD>(-1);
            if((line.size() != 3 || ((kwhile=getKeyword(line[1])) != KEYWORD_WHILE && (ktimes=getKeyword(line[2])) != KEYWORD_TIMES))
                    && (line.size() != 2 || getCondition(line[1]) != COND_ALWAYS))
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1\n%1 [zahl] %2\n%1 %3 [bedingung]\n%1 %4").arg(keywords[KEYWORD_REPEAT]).arg(keywords[KEYWORD_TIMES]).arg(keywords[KEYWORD_WHILE]).arg(conditions[COND_ALWAYS]), current_line);

            //REPEAT ALWAYS
            if(line.size() == 2)
            {
                if(coming_from_repeat_end)
                    loop_count.pop();

                loop_count.push(-1);
                current_line++;
                return;
            }
            if(ktimes == KEYWORD_TIMES)
            {
                if(coming_from_repeat_end)
                {
                    coming_from_repeat_end = false;
                    int count = loop_count.pop();
                    if(count <= 0)
                        current_line = branches[current_line] + 1;
                    else
                    {
                        loop_count.push(count);
                        current_line++;
                    }
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
                return;
            }
            else
            {
                if(coming_from_repeat_end)
                    loop_count.pop();

                bool result;
                if(coming_from_condition)
                {
                    result = condition_exit;
                    coming_from_condition = false;
                }
                else
                {
                    //If not handled, it means current_line is set to the beginning of a custom condition
                    if(!handleCondition(line[2], result))
                        return;
                }

                if(result)
                {
                    current_line++;
                    //Condition, no count
                    loop_count.push(-1);
                    return;
                }
                else
                {
                    current_line = branches[current_line] + 1;
                    return;
                }
            }
            return;
        }
        case KEYWORD_REPEAT_END:
        {
            bool needs_condition = repeat_needs_condition.pop();
            if(line.size() == 3 && !needs_condition)
                throw SteveInterpreterException(QObject::trUtf8("Hier darf keine Bedingung sein, da in Zeile %1 eine angegeben wurde.").arg(branches[current_line]), current_line);

            if(line.size() == 1 && needs_condition)
                throw SteveInterpreterException(QObject::trUtf8("Hier wird eine Bedingung benötigt, da in Zeile %1 keine angegeben wurde.").arg(branches[current_line]), current_line);

            if(line.size() == 3)
            {
                if(getKeyword(line[1]) != KEYWORD_WHILE)
                    throw SteveInterpreterException(QObject::trUtf8("Syntax: %1 %2 [bedingung]").arg(keywords[KEYWORD_REPEAT_END]).arg(keywords[KEYWORD_WHILE]), current_line);

                bool result;
                if(coming_from_condition)
                    result = condition_exit;
                else
                {
                    //If not handled, it means current_line is set to the beginning of a custom condition
                    if(!handleCondition(line[2], result))
                        return;
                }

                if(result)
                    current_line = branches[current_line];
                else
                    current_line++;

                return;
            }
            else if(line.size() == 1)
            {
                int count = loop_count.pop();
                //Count, no condition
                if(count != -1)
                    count--;

                loop_count.push(count);
                current_line = branches[current_line];
                coming_from_repeat_end = true;
                return;
            }
            else
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1 oder %1 %2 [bedingung]").arg(keywords[KEYWORD_REPEAT_END]).arg(keywords[KEYWORD_WHILE]), current_line);

            return;
        }

        case KEYWORD_WHILE:
        {
            if(line.size() != 3 || line[2].compare(keywords[KEYWORD_DO], Qt::CaseInsensitive) != 0)
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1 [bedingung] %2").arg(keywords[KEYWORD_WHILE]).arg(keywords[KEYWORD_DO]), current_line);

            bool result;
            if(coming_from_condition)
            {
                result = condition_exit;
                coming_from_condition = false;
            }
            else
            {
                if(!handleCondition(line[1], result))
                    return;
            }

            if(result)
                current_line++;
            else
                current_line = branches[current_line] + 1;

            return;
        }
        case KEYWORD_WHILE_END:
            if(line.size() != 1)
                throw SteveInterpreterException(QObject::trUtf8("Syntax: %1").arg(keywords[KEYWORD_WHILE_END]), current_line);

            current_line = branches[current_line];
            return;

        case KEYWORD_NEW_INSTR:
        case KEYWORD_NEW_COND:
            //If neither TRUE nor FALSE given, default to TRUE
            condition_exit = true;

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
        std::cout << QObject::trUtf8("Ich würde den nächsten %1-Block betreten.").arg(keywords[KEYWORD_ELSE]).toStdString() << std::endl;
    if(enter_sub)
        std::cout << QObject::trUtf8("Ich würde den nächsten %1- und %2-Block betreten.").arg(keywords[KEYWORD_NEW_INSTR]).arg(keywords[KEYWORD_NEW_COND]).toStdString() << std::endl;
    if(coming_from_repeat_end)
        std::cout << QObject::trUtf8("Ich komme gerade von %1.").arg(keywords[KEYWORD_REPEAT_END]).toStdString() << std::endl;
    if(coming_from_condition)
        std::cout << QObject::trUtf8("Ich komme gerade von einer selbstdefinierten Bedingung. Der Wert ist %1").arg(condition_exit ? "WAHR" : "FALSCH" ).toStdString() << std::endl;
    if(executionFinished())
        std::cout << QObject::trUtf8("Das Programm ist zuende.").toStdString() << std::endl;
}

//Conditions
bool SteveInterpreter::cond_always(World *world, bool has_param, int param)
{
    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return true;
}

bool SteveInterpreter::is_wall(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->isWall();
}

bool SteveInterpreter::is_brick(World *world, bool has_param, int param)
{
    if(!has_param)
        param = 1;

    return world->getStackSize() == param;
}

bool SteveInterpreter::is_marked(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->isMarked();
}

bool SteveInterpreter::is_north(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->getOrientation() == ORIENT_NORTH;
}

bool SteveInterpreter::is_east(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->getOrientation() == ORIENT_EAST;
}

bool SteveInterpreter::is_south(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->getOrientation() == ORIENT_SOUTH;
}

bool SteveInterpreter::is_west(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    return world->getOrientation() == ORIENT_WEST;
}

bool SteveInterpreter::is_full(World *world, bool has_param, int param)
{
    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    //TODO
    return false;
}

bool SteveInterpreter::is_empty(World *world, bool has_param, int param)
{
    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    //TODO
    return false;
}

bool SteveInterpreter::has_bricks(World *world, bool has_param, int param)
{
    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    //TODO
    return false;
}

//Instructions
bool SteveInterpreter::tone(World *world, bool has_param, int param)
{
    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    //Nope!
    return true;
}

bool SteveInterpreter::wait(World *world, bool has_param, int param)
{
    if(!has_param)
        param = 1000;

    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    //TODO
    return true;
}

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

    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    //TODO

    return true;
}

bool SteveInterpreter::putdown(World *world, bool has_param, int param)
{
    if(!has_param)
        param = 1;
    Q_UNUSED(world);
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    //TODO

    return true;
}

bool SteveInterpreter::turnRight(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    world->turnRight();
    return true;
}

bool SteveInterpreter::turnLeft(World *world, bool has_param, int param)
{
    Q_UNUSED(has_param);
    Q_UNUSED(param);

    world->turnLeft();
    return true;
}

bool SteveInterpreter::step(World *world, bool has_param, int param)
{
    if(!has_param)
        param = 1;

    while(param--)
        if(!world->stepForward())
            throw SteveInterpreterException(QObject::trUtf8("Steve war so dumm und wäre beinahe gegen die Wand gelaufen!"), current_line);

    return true;
}

//Other private functions
KEYWORD SteveInterpreter::getKeyword(QString string)
{
    for(auto i : keywords.keys())
    {
        if(keywords[i].compare(string, Qt::CaseInsensitive) == 0)
            return i;
    }

    return static_cast<KEYWORD>(-1);
}

INSTRUCTION SteveInterpreter::getInstruction(QString string)
{
    for(auto i : instructions.keys())
    {
        if(instructions[i].compare(string, Qt::CaseInsensitive) == 0)
            return i;
    }

    return static_cast<INSTRUCTION>(-1);
}

CONDITION SteveInterpreter::getCondition(QString string)
{
    for(auto i : conditions.keys())
    {
        if(conditions[i].compare(string, Qt::CaseInsensitive) == 0)
            return i;
    }

    return static_cast<CONDITION>(-1);
}

//SteveInterpreterException
const char* SteveInterpreterException::what()
{
    if(line_end != line_start)
        return QObject::trUtf8("Fehler in Zeilen %1-%2:\n%3").arg(line_start).arg(line_end).arg(error).toUtf8().data();

    return QObject::trUtf8("Fehler in Zeile %1:\n%2").arg(line_start).arg(error).toUtf8().data();
}

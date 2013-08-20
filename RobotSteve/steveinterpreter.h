/*
 * Author: Fabian Vogt
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 * or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 * Use in public and private schools for educational purposes strongly permitted!
 */

#ifndef STEVEINTERPRETER_H
#define STEVEINTERPRETER_H

#include <exception>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QHash>
#include <QStack>

#include "world.h"

class SteveInterpreter;

class SteveInterpreterException : public std::exception {
public:
    SteveInterpreterException(QString error, int line) : SteveInterpreterException(error, line, line, "") {}
    SteveInterpreterException(QString error, int line, QString affected) : SteveInterpreterException(error, line, line, affected) {}
    SteveInterpreterException(QString error, int line_start, int line_end) : SteveInterpreterException(error, line_start, line_end, "") {}
    ~SteveInterpreterException() throw() {}

    QString getAffected() { return affected; }
    int getLineStart() { return line_start; }
    int getLineEnd() { return line_end; }

    const char* what();

private:
    SteveInterpreterException(QString error, int line_start, int line_end, QString affected) : error(error), line_start(line_start), line_end(line_end), affected(affected) {}

    QString error;
    int line_start;
    int line_end;
    QString affected;
};

typedef bool (SteveInterpreter::*SteveFunctionPtr)(World *world, bool param_given, int param);

class SteveFunction {
public:
    //Needed for QHash
    SteveFunction() : SteveFunction(0, 0, false) {}

    SteveFunction(SteveInterpreter *parent, SteveFunctionPtr function, bool has_param) : SteveFunction(parent, function, has_param, false) {}
    SteveFunction(SteveInterpreter *parent, SteveFunctionPtr function, bool has_param, bool negate_return) : parent(parent), function(function), has_param(has_param), negate_return(negate_return) {}
    bool hasParam() { return has_param; }
    bool operator() (World *world, int param)
    {
        if(negate_return)
            return !(parent->*function)(world, true, param);
        else
            return (parent->*function)(world, true, param);
    }
    bool operator() (World *world)
    {
        if(negate_return)
            return !(parent->*function)(world, false, 1);
        else
            return (parent->*function)(world, false, 1);
    }

private:
    SteveInterpreter *parent;
    SteveFunctionPtr function;
    bool has_param;
    bool negate_return;
};

enum KEYWORD {
    KEYWORD_IF,
    KEYWORD_THEN,
    KEYWORD_ELSE,
    KEYWORD_IF_END,
    KEYWORD_REPEAT,
    KEYWORD_TIMES,
    KEYWORD_REPEAT_END,
    KEYWORD_WHILE,
    KEYWORD_DO,
    KEYWORD_WHILE_END,
    KEYWORD_NEW_INSTR,
    KEYWORD_NEW_INSTR_END,
    KEYWORD_NEW_COND,
    KEYWORD_NEW_COND_END
};

enum INSTRUCTION {
    INSTR_STEP,
    INSTR_TURNLEFT,
    INSTR_TURNRIGHT,
    INSTR_PUTDOWN,
    INSTR_PICKUP,
    INSTR_MARK,
    INSTR_UNMARK,
    INSTR_WAIT,
    INSTR_TONE,
    INSTR_QUIT,
    INSTR_TRUE,
    INSTR_FALSE
};

enum CONDITION {
    COND_ALWAYS,
    COND_NEVER,
    COND_ISWALL,
    COND_NOTISWALL,
    COND_ISBRICK,
    COND_NOTISBRICK,
    COND_MARKED,
    COND_NOTMARKED,
    COND_ISNORTH,
    COND_ISEAST,
    COND_ISSOUTH,
    COND_ISWEST,
    COND_ISFULL,
    COND_NOTISFULL,
    COND_ISEMPTY,
    COND_NOTISEMPTY,
    COND_HASBRICK
};

enum BLOCK {
    BLOCK_IF, BLOCK_ELSE,
    BLOCK_REPEAT,
    BLOCK_WHILE,
    BLOCK_NEW_INSTR,
    BLOCK_NEW_COND
};

struct BlockKeywords {
    KEYWORD begin;
    KEYWORD end;
    BLOCK type;
};

class SteveInterpreter
{
public:
    SteveInterpreter(World *world);
    void setCode(QStringList code) throw (SteveInterpreterException);
    void reset();
    void executeLine() throw (SteveInterpreterException);
    int getLine();
    void dumpCode();
    void setWorld(World *world) { this->world = world; }
    bool executionFinished() { return execution_finished; }

private:
    void findAndThrowMissingBegin(int line, BLOCK block, QString affected = "") throw (SteveInterpreterException);
    bool handleCondition(QString condition_str, bool &result) throw (SteveInterpreterException);
    bool handleInstruction(QString instruction_str) throw (SteveInterpreterException);
    KEYWORD getKeyword(QString string);
    INSTRUCTION getInstruction(QString string);
    CONDITION getCondition(QString string);

    //Conditions:
    bool cond_always(World *world, bool has_param, int param);

    //Instructions:

    //Independant
    World *world;

    //Const after construction
    QHash<KEYWORD, QString> keywords;
    QHash<INSTRUCTION, QString> instructions;
    QHash<CONDITION, QString> conditions;
    QHash<INSTRUCTION, SteveFunction> instruction_functions;
    QHash<CONDITION, SteveFunction> condition_functions;

    //Execution state
    int current_line; // Starts at 0!
    bool coming_from_condition, coming_from_repeat_end, condition_exit, enter_sub, enter_else, execution_finished;
    QStack<int> stack;
    QStack<bool> repeat_needs_condition;
    QStack<int> loop_count;

    //After parse
    QMap<QString, int> custom_instructions, custom_conditions;
    QStringList code;
    QMap<int, QStringList> token;
    QMap<int, int> branches;
    bool code_valid;
    /* 1: WENN NICHTISTWAND TUE (3)
     * 2: SCHRITT
     * 3: SONST (5)
     * 4: RECHTSDREHEN
     * 5: *WENN (1)
     * branches[5] = 1; */
};

#endif // STEVEINTERPRETER_H

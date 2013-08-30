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

    const QString getAffected() { return affected; }
    int getLineStart() { return line_start; }
    int getLineEnd() { return line_end; }

    const char* what();

private:
    SteveInterpreterException(QString error, int line_start, int line_end, QString affected) : error(error), line_start(line_start), line_end(line_end), affected(affected) {}

    const QString error;
    const int line_start;
    const int line_end;
    const QString affected;
};

typedef bool (SteveInterpreter::*SteveFunctionPtr)(World *world, bool param_given, int param);

class SteveFunction {
public:
    //Needed for QHash
    SteveFunction() : SteveFunction(0, 0, false) {}

    SteveFunction(SteveInterpreter *parent, SteveFunctionPtr function, bool has_param) : parent(parent), function(function), has_param(has_param) {}
    bool hasParam() { return has_param; }
    bool operator() (World *world, int param)
    {
        return (parent->*function)(world, true, param);
    }
    bool operator() (World *world)
    {
        return (parent->*function)(world, false, 1);
    }

private:
    SteveInterpreter *parent;
    SteveFunctionPtr function;
    bool has_param;
};

enum KEYWORD {
    KEYWORD_IF,
    KEYWORD_NOT,
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
    INSTR_QUIT,
    INSTR_TRUE,
    INSTR_FALSE
};

enum CONDITION {
    COND_ALWAYS,
    COND_ISWALL,
    COND_ISBRICK,
    COND_MARKED,
    COND_ISNORTH,
    COND_ISEAST,
    COND_ISSOUTH,
    COND_ISWEST
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
    bool isComment(QString s);

    //Conditions:
    bool cond_always(World *world, bool has_param, int param);
    bool is_wall(World *world, bool has_param, int param);
    bool is_brick(World *world, bool has_param, int param);
    bool is_marked(World *world, bool has_param, int param);
    bool is_north(World *world, bool has_param, int param);
    bool is_east(World *world, bool has_param, int param);
    bool is_south(World *world, bool has_param, int param);
    bool is_west(World *world, bool has_param, int param);

    //Instructions:
    bool step(World *world, bool has_param, int param);
    bool turnLeft(World *world, bool has_param, int param);
    bool turnRight(World *world, bool has_param, int param);
    bool deposit(World *world, bool has_param, int param);
    bool pickup(World *world, bool has_param, int param);
    bool mark(World *world, bool has_param, int param);
    bool unmark(World *world, bool has_param, int param);
    bool wait(World *world, bool has_param, int param);

private:
    void findAndThrowMissingBegin(int line, BLOCK block, QString affected = "") throw (SteveInterpreterException);
    bool handleCondition(QString condition_str, bool &result) throw (SteveInterpreterException);
    bool handleInstruction(QString instruction_str) throw (SteveInterpreterException);
    KEYWORD getKeyword(QString string);
    INSTRUCTION getInstruction(QString string);
    CONDITION getCondition(QString string);
    bool match(QString &str, KEYWORD keyword);
    bool match(QString &str, INSTRUCTION instr);
    bool match(QString &str, CONDITION cond);

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
    bool coming_from_condition, coming_from_repeat_end, enter_sub, enter_else, execution_finished;
    QStack<int> stack;
    QStack<int> loop_count;
    QStack<bool> custom_condition_return_stack;

    //After parse
    QHash<QString, int> custom_instructions, custom_conditions;
    QStringList code;
    bool code_valid;
    QMap<int, QStringList> token;
    QMap<int, int> branches;
    /* 1: WENN NICHTISTWAND TUE (3)
     * 2: SCHRITT
     * 3: SONST (5)
     * 4: RECHTSDREHEN
     * 5: *WENN (1)
     * branches[5] = 1; */
};

#endif // STEVEINTERPRETER_H

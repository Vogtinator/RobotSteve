#ifndef STEVEINTERPRETER_H
#define STEVEINTERPRETER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QStack>

#include "world.h"

class SteveInterpreter;

typedef void (SteveInterpreter::*InstructionFunction)(World *world, bool param_given, int param);
typedef bool (SteveInterpreter::*ConditionFunction)(World *world, bool param_given, int param);

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
    INSTR_QUIT
};

enum CONDITION {
    COND_ALWAYS,
    COND_ISWALL,
    COND_NOTISWALL,
    COND_ISBRICK,
    COND_NOTISBRICK,
    COND_MARKED,
    COND_NOTMARKED,
    COND_ISNORTH,
    COND_ISEAST,
    COND_SOUTH,
    COND_ISWEST,
    COND_ISFULL,
    COND_NOTISFULL,
    COND_ISEMPTY,
    COND_NOTISEMPTY,
    COND_HASBRICK
};

enum BLOCK {
    BLOCK_IF,
    BLOCK_REPEAT,
    BLOCK_WHILE,
    BLOCK_INSTR,
    BLOCK_COND
};

class SteveInterpreter
{
    friend class InterpreterBlock;

public:
    SteveInterpreter();
    void setCode(QStringList code) throw (QString);
    void reset();
    void executeLine() throw (QString);
    QString getError();
    int getLine();

private:
    int start_line, current_line; // Starts at 0!
    QString error;
    QMap<KEYWORD, QString> keywords;
    QMap<INSTRUCTION, QString> instructions;
    QMap<CONDITION, QString> conditions;
    QMap<QString, int> customInstructions, customConditions;
    QStringList code;
    QStack<int> stack;
    QMap<int, int> branches;
    /* 1: WENN NICHTISTWAND TUE (3)
     * 2: SCHRITT
     * 3: SONST (5)
     * 4: RECHTSDREHEN
     * 5: *WENN (1)
     * branches[5] = 1; */
};

#endif // STEVEINTERPRETER_H

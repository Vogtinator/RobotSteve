#ifndef STEVEINTERPRETER_H
#define STEVEINTERPRETER_H

#include <exception>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QStack>

#include "world.h"

class SteveInterpreter;

class SteveInterpreterException : public std::exception {
public:
    SteveInterpreterException(QString error, int line) : SteveInterpreterException(error, line, line, "") {}
    SteveInterpreterException(QString error, int line, QString affected) : SteveInterpreterException(error, line, line, affected) {}
    SteveInterpreterException(QString error, int line_start, int line_end) : SteveInterpreterException(error, line_start, line_end, "") {}
    ~SteveInterpreterException() throw() {}

    const char* what()
    {
        if(line_end != line_start)
            return QObject::trUtf8("Fehler in Zeilen %1-%2:\n%3").arg(line_start).arg(line_end).arg(error).toUtf8().data();

        return QObject::trUtf8("Fehler in Zeile %1:\n%2").arg(line_start).arg(error).toUtf8().data();
    }

    QString getAffected() { return affected; }
    int getLineStart() { return line_start; }
    int getLineEnd() { return line_end; }

private:
    SteveInterpreterException(QString error, int line_start, int line_end, QString affected) : error(error), line_start(line_start), line_end(line_end), affected(affected) {}

    QString error;
    int line_start;
    int line_end;
    QString affected;
};

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
    SteveInterpreter();
    void setCode(QStringList code) throw (SteveInterpreterException);
    void reset();
    void executeLine() throw (QString);
    QString getError();
    int getLine();
    void dumpCode();

private:
    void findAndThrowMissingBegin(int line, BLOCK block, QString affected = "") throw (SteveInterpreterException);

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

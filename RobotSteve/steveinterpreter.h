#ifndef STEVEINTERPRETER_H
#define STEVEINTERPRETER_H

#include <exception>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QHash>
#include <QStack>
#include <QPixmap>

#include "world.h"

class SteveInterpreter;

class SteveInterpreterException : public std::exception {
public:
    SteveInterpreterException(const QString &error, int line) : SteveInterpreterException(error, line, "") {}
    SteveInterpreterException(const QString &error, int line, const QString &affected) : error(error), line(line), affected(affected) {}
    ~SteveInterpreterException() throw() {}

    const QString &getAffected() { return affected; }
    int getLine() { return line; }

    QString what();

private:

    const QString error;
    const int line;
    const QString affected;
};

typedef bool (SteveInterpreter::*SteveFunctionPtr)(World *world, bool param_given, int param);

class SteveFunction {
public:
    //Needed for QHash
    SteveFunction() : SteveFunction(0, 0, false) {}

    SteveFunction(SteveInterpreter *parent, SteveFunctionPtr function, bool has_param) : parent(parent), function(function), has_param(has_param) {}
    bool hasParam() const { return has_param; }
    bool operator() (World *world, int param) const
    {
        return (parent->*function)(world, true, param);
    }
    bool operator() (World *world) const
    {
        return (parent->*function)(world, false, 1);
    }

private:
    SteveInterpreter *parent;
    SteveFunctionPtr function;
    bool has_param;
};

enum BLOCK {
    BLOCK_IF, BLOCK_ELSE,
    BLOCK_REPEAT,
    BLOCK_WHILE,
    BLOCK_NEW_INSTR,
    BLOCK_NEW_COND
};

class SteveInterpreter : public QObject
{
    Q_OBJECT

    Q_ENUMS(KEYWORD)
    Q_ENUMS(INSTRUCTION)
    Q_ENUMS(CONDITION)

    friend class SteveHighlighter;

public:
    SteveInterpreter(World *world);

    void setCode(QStringList code) throw (SteveInterpreterException);
    void reset();
    void executeLine() throw (SteveInterpreterException);
    int getLine();
    void dumpCode();
    void setWorld(World *world) { this->world = world; }
    bool executionFinished() { return execution_finished; }
    bool hitBreakpoint() { return hit_breakpoint; }
    QPixmap structureChart()  throw (SteveInterpreterException);

    //Conditions:
    bool condAlways(World *world, bool has_param, int param);
    bool isWall(World *world, bool has_param, int param);
    bool isCube(World *world, bool has_param, int param);
    bool isBrick(World *world, bool has_param, int param);
    bool isMarked(World *world, bool has_param, int param);
    bool isNorth(World *world, bool has_param, int param);
    bool isEast(World *world, bool has_param, int param);
    bool isSouth(World *world, bool has_param, int param);
    bool isWest(World *world, bool has_param, int param);

    //Instructions:
    bool step(World *world, bool has_param, int param);
    bool turnLeft(World *world, bool has_param, int param);
    bool turnRight(World *world, bool has_param, int param);
    bool deposit(World *world, bool has_param, int param);
    bool pickup(World *world, bool has_param, int param);
    bool mark(World *world, bool has_param, int param);
    bool unmark(World *world, bool has_param, int param);
    bool breakpoint(World *world, bool has_param, int param);

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
        KEYWORD_BREAK,
        KEYWORD_CONTINUE,
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
        INSTR_QUIT,
        INSTR_TRUE,
        INSTR_FALSE,
        INSTR_BREAKPOINT
    };

    enum CONDITION {
        COND_ALWAYS,
        COND_WALL,
        COND_CUBE,
        COND_BRICK,
        COND_MARKED,
        COND_NORTH,
        COND_EAST,
        COND_SOUTH,
        COND_WEST
    };

    KEYWORD getKeyword(QString string);
    INSTRUCTION getInstruction(QString string);
    CONDITION getCondition(QString string);
    const QString &str(KEYWORD keyword);
    const QString &str(INSTRUCTION instr);
    const QString &str(CONDITION cond);

private:
    void findAndThrowMissingBegin(int line, BLOCK block, const QString &affected = "") throw (SteveInterpreterException);
    bool handleCondition(QString condition_str, bool &result) throw (SteveInterpreterException);
    bool handleInstruction(QString instruction_str) throw (SteveInterpreterException);
    bool isComment(const QString &s);
    template <typename TOKEN> bool match(const QString &str, const TOKEN tok);

    //Structure chart generation
    void drawText(int x, int y, QPainter &painter, const QString &text);
    int textWidth(const QString &text);
    int textHeight();
    QPixmap structureChartBlock(const struct StructureBlock &sb);
    QPixmap structureChartIfBlock(const struct StructureBlock &sb);
    QPixmap structureChartOtherBlock(const struct StructureBlock &sb);

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
    bool coming_from_condition, coming_from_repeat_end, enter_sub, enter_else, execution_finished, hit_breakpoint;
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

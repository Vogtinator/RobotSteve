#ifndef STEVEEDIT_H
#define STEVEEDIT_H

#include <QPlainTextEdit>

#include "steveinterpreter.h"

class SteveEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit SteveEdit(SteveInterpreter *interpreter, QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    SteveInterpreter *interpreter;
    QHash<SteveInterpreter::KEYWORD, QString> keyword_help;
    QHash<SteveInterpreter::INSTRUCTION, QString> instruction_help;
    QHash<SteveInterpreter::CONDITION, QString> condition_help;
};

#endif // STEVEEDIT_H

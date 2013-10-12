#ifndef STEVEEDIT_H
#define STEVEEDIT_H

#include <QPlainTextEdit>

#include "steveinterpreter.h"
#include "stevehelp.h"

class SteveEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit SteveEdit(SteveHelp *help, QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    SteveHelp *help;

};

#endif // STEVEEDIT_H

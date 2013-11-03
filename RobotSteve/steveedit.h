#ifndef STEVEEDIT_H
#define STEVEEDIT_H

#include <QTextEdit>
#include <QCompleter>

#include "steveinterpreter.h"
#include "stevehelp.h"

class SteveEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit SteveEdit(SteveHelp *help, QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void insertCompletion(const QString& completion);

private:
    QString currentWord();

    SteveHelp *help;
    QCompleter completer;

};

#endif // STEVEEDIT_H

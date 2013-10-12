#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

#include "stevehighlighter.h"

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QString html, SteveInterpreter *interpreter, QWidget *parent = 0);
    ~HelpDialog();

private:
    Ui::HelpDialog *ui;
    SteveHighlighter *highlighter;
};

#endif // HELPDIALOG_H

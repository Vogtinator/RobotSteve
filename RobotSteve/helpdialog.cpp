#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QString html, SteveInterpreter *interpreter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);

    ui->textEdit->setHtml(html);
    highlighter = new SteveHighlighter(ui->textEdit, interpreter);

}

HelpDialog::~HelpDialog()
{
    delete ui;
    delete highlighter;
}

#include <iostream>
#include <QTextDocumentFragment>
#include <QTimer>
#include <QXmlStreamReader>
#include <QMetaEnum>
#include <QToolTip>

#include "steveedit.h"
#include "steveinterpreter.h"

SteveEdit::SteveEdit(SteveHelp *help, QWidget *parent) :
    QTextEdit(parent), help{help}
{

}

void SteveEdit::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_F1)
    {
        QTextCursor cursor = textCursor();
        cursor.select(QTextCursor::WordUnderCursor);
        QString word = cursor.selection().toPlainText();

        QPoint global = mapToGlobal({this->cursorRect().x(), this->cursorRect().y() + this->cursorRect().height()});

        QToolTip::showText(global, QString("<b>%1</b>%2").arg(word).arg(Qt::convertFromPlainText(help->getHelp(word))), this);
    }
    else if(e->key() == Qt::Key_Escape)
        QToolTip::hideText();

    else
        QTextEdit::keyPressEvent(e);
}

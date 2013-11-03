#include <iostream>
#include <QTextDocumentFragment>
#include <QTimer>
#include <QXmlStreamReader>
#include <QMetaEnum>
#include <QToolTip>
#include <QKeyEvent>
#include <QScrollBar>
#include <QTextBlock>
#include <QAbstractItemView>
#include <QStringListModel>

#include "steveedit.h"
#include "steveinterpreter.h"

SteveEdit::SteveEdit(SteveHelp *help, QWidget *parent) :
    QTextEdit(parent), help{help}, completer{help->getWordList()}
{
    completer.setCaseSensitivity(Qt::CaseInsensitive);
    completer.setWrapAround(false);
    completer.setWidget(this);
    completer.setCompletionMode(QCompleter::PopupCompletion);

    connect(&completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void SteveEdit::insertCompletion(const QString& completion)
{
    QTextCursor cursor = textCursor();
    int extra = completion.length() - completer.completionPrefix().length();

    cursor.movePosition(QTextCursor::Left);
    cursor.movePosition(QTextCursor::EndOfWord);
    cursor.insertText(completion.right(extra));
    setTextCursor(cursor);
}

//QTextCursor.select(WORD_UNDER_CURSOR) also sees "*" as seperator.
//This function only respects QChar::isSpace
QString SteveEdit::currentWord()
{
    QTextCursor cursor = textCursor();
    QString current_block_text = cursor.block().text();
    int end_pos = cursor.positionInBlock(), start_pos = cursor.positionInBlock();

    while(end_pos < current_block_text.length() && !current_block_text[end_pos].isSpace())
        end_pos++;

    while(start_pos >= 0 && !current_block_text[start_pos].isSpace())
        start_pos--;

    return current_block_text.mid(start_pos, end_pos - start_pos).trimmed();
}

void SteveEdit::keyPressEvent(QKeyEvent *e)
{
    if(completer.popup()->isVisible())
    {
        if(!e->modifiers())
        {
            switch(e->key())
            {
            case Qt::Key_Down:
            case Qt::Key_Up:
            case Qt::Key_Return:
            case Qt::Key_Enter:
            case Qt::Key_PageDown:
            case Qt::Key_PageUp:
            case Qt::Key_Escape:
                e->ignore();
                return;
            default:
                break;
            }
        }
    }

    if(e->key() == Qt::Key_F1)
    {
        QString current_word = currentWord();
        QPoint global = mapToGlobal({this->cursorRect().x(), this->cursorRect().y() + this->cursorRect().height()});

        QToolTip::showText(global, QString("<b>%1</b>%2").arg(current_word).arg(Qt::convertFromPlainText(help->getHelp(current_word))), this);
    }
    else if(e->key() == Qt::Key_Escape)
        QToolTip::hideText();

    else
    {
        QTextEdit::keyPressEvent(e);

        if(e->text().length() == 0)
            return;

        QString current_word = currentWord();
        if(current_word.length() == 0)
        {
            completer.popup()->close();
            return;
        }

        completer.setCompletionPrefix(current_word);
        completer.popup()->setCurrentIndex(completer.completionModel()->index(0, 0));
        completer.popup()->show();

        QRect cr = cursorRect();
        cr.setWidth(completer.popup()->sizeHintForColumn(0)
                    + completer.popup()->verticalScrollBar()->sizeHint().width());
        completer.complete(cr);
    }
}

#ifndef STEVEHIGHLIGHTER_H
#define STEVEHIGHLIGHTER_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <steveinterpreter.h>

enum Token {
    TOK_KEYWORD = 0,
    TOK_CONDITION,
    TOK_INSTRUCTION
};

class SteveHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SteveHighlighter(QPlainTextEdit *editor, SteveInterpreter *interpreter);

    void highlightBlock(const QString &text) override;
    void highlight(int line, const QTextCharFormat &format, const QString &what = "");
    void resetHighlight();
    void setFormat(Token what, const QTextCharFormat &format);

public slots:
    //This rehighlight doesn't trigger textChanged()
    void rehighlight();

private:
    int highlight_line;
    QString highlight_str;
    SteveInterpreter *interpreter;
    QPlainTextEdit *parent;
    QTextCharFormat format[TOK_INSTRUCTION + 1], highlight_format;

};

#endif // STEVEHIGHLIGHTER_H

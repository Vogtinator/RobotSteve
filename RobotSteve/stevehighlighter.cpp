#include "stevehighlighter.h"

SteveHighlighter::SteveHighlighter(QTextEdit *editor, SteveInterpreter *interpreter)
    : QSyntaxHighlighter{editor->document()}, highlight_line{-1}, interpreter{interpreter}, parent{editor}
{
    QTextCharFormat format;

    format.setForeground(QColor(0, 128, 0));
    format.setFontWeight(QFont::Bold);
    setFormat(TOK_KEYWORD, format);

    format.setForeground(QColor(192, 16, 112));
    format.setFontWeight(QFont::Bold);
    setFormat(TOK_CONDITION, format);

    format.setForeground(QColor(128, 0, 0));
    format.setFontWeight(QFont::Bold);
    setFormat(TOK_INSTRUCTION, format);
}

void SteveHighlighter::highlightBlock(const QString &text)
{
    int line;
    if(previousBlockState() == -1)
        line = 0;

    else
        line = previousBlockState();

    QStringList lines = text.split("\n");
    QString line_str;
    int line_start = 0;

    for(int l = 0; l < lines.length(); line_start += line_str.length(), l++, line++)
    {
        line_str = lines[l];
        int highlight_start = -1, highlight_end = -1;

        if(line == highlight_line)
        {
            if(highlight_str.isEmpty())
            {
                //Don't set highlight_start and _end here, we still want it highlighted above the background color
                QTextEdit::ExtraSelection extra_selection;
                extra_selection.cursor = QTextCursor{parent->document()};
                extra_selection.cursor.setPosition(line_start + currentBlock().position());
                highlight_format.setProperty(QTextFormat::FullWidthSelection, true);
                extra_selection.format = highlight_format;

                QList<QTextEdit::ExtraSelection> extra_selections {extra_selection};
                parent->setExtraSelections(extra_selections);
            }
            else
            {
                parent->setExtraSelections({});
                highlight_start = line_str.indexOf(highlight_str);
                highlight_end = highlight_start + highlight_str.length();
                highlight_format.setProperty(QTextFormat::FullWidthSelection, false);
                QSyntaxHighlighter::setFormat(line_start + highlight_start, highlight_str.length(), highlight_format);

            }
        }

        if(interpreter->isComment(line_str))
            continue; //Don't highlight comments

        QString current_token;
        int current_token_start = line_start;

        for(int pos = 0; pos < line_str.length(); pos++)
        {
            QChar c = line_str.at(pos);

            if(!c.isSpace() && c != '(' && c != ')')
                current_token.append(c);

            if(c.isSpace() || c == '(' || c == ')' || pos == line_str.length() - 1)
            {
                current_token = current_token.toLower();

                if(current_token.length() > 0 && !(pos >= highlight_start && pos <= highlight_end))
                {
                    if(interpreter->getKeyword(current_token) != -1)
                        QSyntaxHighlighter::setFormat(current_token_start, current_token.length(), format[TOK_KEYWORD]);
                    else if(interpreter->getCondition(current_token) != -1)
                        QSyntaxHighlighter::setFormat(current_token_start, current_token.length(), format[TOK_CONDITION]);
                    else if(interpreter->getInstruction(current_token) != -1)
                        QSyntaxHighlighter::setFormat(current_token_start, current_token.length(), format[TOK_INSTRUCTION]);
                    else if(interpreter->custom_instructions.contains(current_token))
                        QSyntaxHighlighter::setFormat(current_token_start, current_token.length(), format[TOK_INSTRUCTION]);
                    else if(interpreter->custom_conditions.contains(current_token))
                        QSyntaxHighlighter::setFormat(current_token_start, current_token.length(), format[TOK_CONDITION]);
                }

                current_token.clear();
                current_token_start = line_start + pos + 1;
            }
        }
    }

    setCurrentBlockState(line);
}

void SteveHighlighter::setFormat(Token what, const QTextCharFormat &format)
{
    this->format[what] = format;
    rehighlight();
}

void SteveHighlighter::highlight(int line, const QTextCharFormat &format, const QString &what)
{
    highlight_line = line;
    highlight_format = format;
    highlight_str = what;

    rehighlight();
}

void SteveHighlighter::resetHighlight()
{
    if(highlight_line < 0)
        return;

    highlight_line = -1;

    parent->setExtraSelections({});
    rehighlight();
}

void SteveHighlighter::rehighlight()
{
    parent->blockSignals(true);
    QSyntaxHighlighter::rehighlight();
    parent->blockSignals(false);
}

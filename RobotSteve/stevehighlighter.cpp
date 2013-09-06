#include "stevehighlighter.h"

SteveHighlighter::SteveHighlighter(QPlainTextEdit *editor, SteveInterpreter *interpreter)
    : QSyntaxHighlighter(editor->document()), interpreter(interpreter), parent(editor)
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
    int line, document_pos;
    if(previousBlockState() == -1)
    {
        line = 0;
        document_pos = 0;
    }
    else
    {
        line = previousBlockState() & 0xFFFF;
        document_pos = (previousBlockState() >> 16) & 0xFFFF;
    }

    int line_start = 0;
    QString current_token;
    int current_token_start = 0;
    int highlight_start = -1, highlight_end = -1;
    bool line_is_comment = false;

    for(int pos = 0; pos < text.length(); pos++)
    {
        QChar c = text.at(pos);
        if(c == '\n')
            line++;

        if(c == '\n' || pos == 0)
        {
            if(line == highlight_line)
            {
                if(highlight_str.isEmpty())
                {
                    //TODO: text not always single line
                    //Don't set highlight_start and _end here, we still want it highlighted above the background color
                    QTextEdit::ExtraSelection extra_selection;
                    extra_selection.cursor = QTextCursor{parent->document()};
                    extra_selection.cursor.setPosition(document_pos + line_start);
                    highlight_format.setProperty(QTextFormat::FullWidthSelection, true);
                    extra_selection.format = highlight_format;

                    QList<QTextEdit::ExtraSelection> extra_selections {extra_selection};
                    parent->setExtraSelections(extra_selections);
                }
                else
                {
                    parent->setExtraSelections({});
                    highlight_start = text.indexOf(highlight_str);
                    highlight_end = highlight_start + highlight_str.length();
                    highlight_format.setProperty(QTextFormat::FullWidthSelection, false);
                    QSyntaxHighlighter::setFormat(highlight_start, highlight_str.length(), highlight_format);

                }
            }

            line_is_comment = false;
            line_start = pos;
        }

        if(line_is_comment)
            continue;

        if(!text.at(pos).isSpace() && !(text.at(pos) == '\n') && c != '(' && c != ')')
            current_token.append(text.at(pos));

        if(c.isSpace() || c == '(' || c == ')' || pos == text.length() - 1)
        {
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
                else if(interpreter->isComment(current_token))
                    line_is_comment = true;
            }

            current_token.clear();
            current_token_start = pos + 1;
        }
    }

    document_pos += text.length();

    //TODO: Remove this ugly crap
    setCurrentBlockState(((line+1) & 0xFFFF) | (((document_pos+1) & 0xFFFF) << 16));
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

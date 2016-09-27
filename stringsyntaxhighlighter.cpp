#include "stringsyntaxhighlighter.h"

stringSyntaxHighlighter::stringSyntaxHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
//http://doc.qt.io/qt-4.8/qt-richtext-syntaxhighlighter-example.html
    stringFormat.setFontWeight(QFont::Bold);
    stringFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("$?STR_[^\"')]+");
    rule.format = stringFormat;
    highlightingRules.append(rule);
}

void stringSyntaxHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);
}

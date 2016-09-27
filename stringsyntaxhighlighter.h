#ifndef STRINGSYNTAXHIGHLIGHTER_H
#define STRINGSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
class stringSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    stringSyntaxHighlighter(QTextDocument* parent);
protected:
     void highlightBlock(const QString &text);
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat stringFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QRegExp commentStartExpression;
       QRegExp commentEndExpression;
};

#endif // STRINGSYNTAXHIGHLIGHTER_H

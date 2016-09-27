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
};

#endif // STRINGSYNTAXHIGHLIGHTER_H

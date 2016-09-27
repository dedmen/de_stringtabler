#include "stringsyntaxhighlighter.h"
#include <QStringList>
stringSyntaxHighlighter::stringSyntaxHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
//http://doc.qt.io/qt-4.8/qt-richtext-syntaxhighlighter-example.html
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);




    stringFormat.setFontWeight(QFont::Bold);
    stringFormat.setForeground(Qt::magenta);
    rule.pattern = QRegExp("$?STR_[^\"')]+");//translated strings
    rule.format = stringFormat;
    highlightingRules.append(rule);


     keywordFormat.setForeground(Qt::red);
       keywordFormat.setFontWeight(QFont::Bold);
       QStringList keywordPatterns;
       keywordPatterns << "\\bif\\b" << "\\bthen\\b" << "\\bcall\\b"
                       << "\\bparams\\b" << "\\bformat\\b" << "\\blocalize\\b"
                       << "\\bselect\\b" << "\\bprivate\\b" << "\\bwhile\\b" << "\\bdo\\b"
                       << "\\bget[tT]ext\\b" << "\\bconfig[fF]ile\\b" << "\\bset\\b"
                       << "\\bcount\\b" << " !?= ";
       foreach (const QString &pattern, keywordPatterns) {
           rule.pattern = QRegExp(pattern);
           rule.format = keywordFormat;
           highlightingRules.append(rule);
       }

       singleLineCommentFormat.setForeground(Qt::darkGreen);
          rule.pattern = QRegExp("//[^\n]*");
          rule.format = singleLineCommentFormat;
          highlightingRules.append(rule);

          multiLineCommentFormat.setForeground(Qt::darkGreen);

          commentStartExpression = QRegExp("/\\*");
          commentEndExpression = QRegExp("\\*/");



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


    int startIndex = 0;
     if (previousBlockState() != 1)
         startIndex = commentStartExpression.indexIn(text);
     while (startIndex >= 0) {
            int endIndex = commentEndExpression.indexIn(text, startIndex);
            int commentLength;
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex
                                + commentEndExpression.matchedLength();
            }
            setFormat(startIndex, commentLength, multiLineCommentFormat);
            startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
        }
}

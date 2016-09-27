#ifndef STRINGTABLECONFLICTFINDER_H
#define STRINGTABLECONFLICTFINDER_H

#include <QObject>
#include "stringtableentry.h"
enum class conflictType {
    missingEntry,
    todoEntry,
    wrongTable
};

struct stringtableConflict{


    conflictType type;
    QString keyName;
    QString filePath;
    int lineNumber;
QString belongsToStringtable;
QSharedPointer<stringTableBase> elementPtr;

};

class stringtableConflictFinder : public QObject
{
    Q_OBJECT
public:
    explicit stringtableConflictFinder(QObject *parent = 0);
    void setStringTables(QVector<QSharedPointer<stringTable>> pStringTables){stringTables = pStringTables;}
    void setBasePath(QString basePath_);
    void run();
signals:
    void conflictFound(stringtableConflict);
public slots:
private:
    QStringList processFile(QString filePath);
    QString basePath;
    QVector<QSharedPointer<stringTable>> stringTables;
    QVector<stringtableConflict> conflicts;
};

#endif // STRINGTABLECONFLICTFINDER_H

#include "stringtableconflictfinder.h"
#include <QDirIterator>
#include <QRegularExpressionMatchIterator>
#include <QStringList>
#include <QDebug>
stringtableConflictFinder::stringtableConflictFinder(QObject *parent) : QObject(parent)
{

}

void stringtableConflictFinder::setBasePath(QString basePath_)
{
    basePath = basePath_;
}

void stringtableConflictFinder::run()
{

    QMap<QString,QStringList> definedKeys;
    for (QSharedPointer<stringTable> &stringTable: stringTables){
        for (QSharedPointer<stringTablePackage> &package: stringTable->getPackages()){
            QVector<QSharedPointer<stringTableContainer>> containers =  package->getContainers();
            for (QSharedPointer<stringTableContainer> &container: containers){
                QVector<QSharedPointer<stringTableEntry>> entries =  container->getEntries();
                for (QSharedPointer<stringTableEntry> &entry: entries){
                    definedKeys[stringTable->getName()] << entry->getName();
                }
            }
        }
    }




    QDirIterator it(basePath, QStringList() << "*.sqf" << "*.ext" << "*.cpp" << "*.hpp", QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    QStringList usedKeys;
    QVector<QSharedPointer<stringTable>> tablesVector;
    while (it.hasNext()){
        QString path = it.next();
        QStringList foundKeys = processFile(path);
        foundKeys.removeDuplicates();
        //#TODO cant do matching in here. Have to return proper struct with filePath and line where a key was found
        for (QString table : definedKeys.keys()){
            if (path.contains("/"+table.left(table.indexOf("/"))+"/")){
                for (QString key : definedKeys[table])
                    foundKeys.removeAll(key);
            } else {

                for (QString foundKey : foundKeys){
                    if (definedKeys[table].contains(foundKey)){
                        foundKeys.removeAll(foundKey);
                        stringtableConflict conflict;
                        conflict.type = conflictType::wrongTable;
                        conflict.filePath = path;
                        conflict.keyName = foundKey;

                        for (QSharedPointer<stringTable> &stable : stringTables){
                            if (stable->findByName(foundKey))
                                conflict.elementPtr = stable->findByName(foundKey);
                            if (stable->isInSubfolder(path))
                                conflict.belongsToStringtable = stable->getName();
                        }

                        if ( conflict.belongsToStringtable.isEmpty())
                            break;//Cant be wrong stringtable because it doesnt belong to any we know
                        conflict.lineNumber = 0;
                        qDebug() << "conflict" << (int)conflict.type << conflict.filePath << conflict.keyName << "should be" << conflict.belongsToStringtable;
                        conflicts.push_back(conflict);



                    }
                }







            }
        }
        usedKeys+=foundKeys;
    }
    usedKeys.removeDuplicates();


    QStringList missingKeys = usedKeys;


    //
    //QStringList wildcards = QtPrivate::QStringList_filter(&missingKeys,QRegularExpression("[*%]"));
    //for (QString& wildcardedString : wildcards){
    //    QString copyString = wildcardedString;
    //    if (!wildcardedString.left(2).compare("$*") ||  !wildcardedString.left(1).compare("*")){
    //        if (!definedKeys.filter(QRegularExpression(copyString.replace("*","STR_.*_.*_"))).isEmpty())
    //            definedKeys << wildcardedString;
    //    } else if (wildcardedString.contains("%")){
    //
    //        if (!definedKeys.filter(QRegularExpression(copyString.replace(QRegularExpression("%[0-9]*"),".*"))).isEmpty())
    //            definedKeys << wildcardedString;
    //    }
    //}
    //for (QString &it : definedKeys){
    //    missingKeys.removeAll(it);
    //}
    //
    // qDebug() << missingKeys;
    //QStringList defCopy = definedKeys;
    //for (QString &it : usedKeys){
    //    defCopy.removeAll(it);
    //}
    //qDebug() << defCopy;



}
#include <QBuffer>
QStringList stringtableConflictFinder::processFile(QString filePath)
{
    QStringList foundKeys;
    QFile inputFile(filePath);
    inputFile.open(QFile::ReadOnly);
    QString content = QString::fromUtf8(inputFile.readAll());
    QRegularExpression regExp("[\"']\\$?(STR_[^\"']*)[\"']");


    QRegularExpressionMatchIterator i = regExp.globalMatch(content);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(1);
        //  qDebug() << "normal" << word;
        foundKeys << word;
        for (QSharedPointer<stringTable> &stable : stringTables){
            QSharedPointer<stringTableEntry> x = qSharedPointerDynamicCast<stringTableEntry>(stable->findByName(word));
            if (x)
                x->addUsage({filePath,match.capturedStart(1),match.capturedEnd(1)});
        }
    }

    QRegularExpression regExpMacros("([LC])STRING\\((.*?)\\)");

    i = regExpMacros.globalMatch(content);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString type = match.captured(1);
        QString word = match.captured(2);
        // qDebug() << "wildcard" << type << word;
        if (type.compare("C"))
            foundKeys << "$*"+word;
        else
            foundKeys << "*"+word;
    }


    QRegularExpression regExpTodo("(.*)\"[^\\n\\r]*#Stringtable");

    i = regExpTodo.globalMatch(content);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        stringtableConflict conflict;
        conflict.type = conflictType::todoEntry;
        conflict.filePath =filePath;
        QByteArray fileContent = content.toUtf8();
        QBuffer buffer(&fileContent);
        buffer.open(QIODevice::ReadOnly);
        QByteArray word = match.captured(1).toUtf8();
        int lineNumber = 1;
        while (!buffer.readLine().contains(word))
            ++lineNumber;

        conflict.lineNumber =lineNumber;
        qDebug() << "conflict" << (int)conflict.type << conflict.filePath << conflict.lineNumber;
        conflicts.push_back(conflict);
    }




    return foundKeys;
}

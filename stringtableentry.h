#ifndef STRINGTABLEENTRY_H
#define STRINGTABLEENTRY_H

#include <QObject>
#include <QSharedPointer>
#include <QMap>
#include <QVector>

enum class language {
    Original,
    English,
    Czech,
    French,
    German,
    Italian,
    Polish,
    Portuguese,
    Russian,
    Spanish,
    Korean,
    Japanese,
    Turkish,
    invalid
};
class stringTableEntry;
class stringTableContainer;
class stringTable;

struct fileLocation {
    QString filePath;
    int offset;
    int endoffset;
};


class stringTableBase : public QObject
{
    Q_OBJECT
public:
    explicit stringTableBase(QObject *parent = 0) : QObject(parent){}
    void setName(QString _name) {name = _name;}
    QString getName() {return name;}
    void setTag(QString tag,QString value){tags[tag] = value;}
    bool removeTag(QString tag) {return tags.remove(tag) > 0;}
    QMap<QString,QString> getTags(){return tags;}
signals:

public slots:
private:
    QString name;
    QMap<QString,QString> tags;
};

class stringTablePackage : public stringTableBase , public QEnableSharedFromThis<stringTablePackage>
{
    Q_OBJECT
public:
    explicit stringTablePackage(QObject *parent = 0) : stringTableBase(parent) {}
    void setTable(QSharedPointer<stringTable>& pTable){table = pTable;}
    QSharedPointer<stringTable> getTable() {return table;}
    void addContainer(QSharedPointer<stringTableContainer>& pContainer);
    void removeContainer(QSharedPointer<stringTableContainer>& pContainer){containers.removeAll(pContainer);}
    QVector<QSharedPointer<stringTableContainer>> getContainers(){return containers;}
signals:

public slots:
private:
    QSharedPointer<stringTable> table;
    QVector<QSharedPointer<stringTableContainer>> containers;
};

class stringTableContainer : public stringTableBase, public QEnableSharedFromThis<stringTableContainer>
{
    Q_OBJECT
public:
    explicit stringTableContainer(QObject *parent = 0) : stringTableBase(parent) {}
    void setPackage(QSharedPointer<stringTablePackage>& pPackage) {package = pPackage;}
    QSharedPointer<stringTablePackage> getPackage(){return package;}
    void addEntry(QSharedPointer<stringTableEntry>& pEntry);
    void removeEntry(QSharedPointer<stringTableEntry>& pEntry){entries.removeAll(pEntry);}
    QVector<QSharedPointer<stringTableEntry>> getEntries(){return entries;}
signals:

public slots:
private:
    QSharedPointer<stringTablePackage> package;
    QVector<QSharedPointer<stringTableEntry>> entries;
};


class stringTableEntry : public stringTableBase, public QEnableSharedFromThis<stringTableEntry>
{
    Q_OBJECT
public:
    explicit stringTableEntry(QObject *parent = 0) : stringTableBase(parent) {}
    //Returns if a new entry was added
    bool setTranslation(language lang,QString translation);
    QMap<language,QString> getTranslations(){return translations; }
    void clearTranslations();
    void setContainer(QSharedPointer<stringTableContainer>& pContainer) {container = pContainer;}
    QSharedPointer<stringTableContainer> getContainer(){return container;}
    QVector<fileLocation> getUsages(){return usages;}
    void addUsage(fileLocation path){usages << path;}
signals:

public slots:
private:
    QSharedPointer<stringTableContainer> container;
    QVector<fileLocation> usages;
    QMap<language,QString> translations;
};

class stringTable : public stringTableBase, public QEnableSharedFromThis<stringTable>
{
    Q_OBJECT
public:
    explicit stringTable(QObject *parent = 0) : stringTableBase(parent) {}
    void addPackage(QSharedPointer<stringTablePackage>& pPackage);
    void removePackage(QSharedPointer<stringTablePackage>& pPackage){packages.removeAll(pPackage);}
    QVector<QSharedPointer<stringTablePackage>> getPackages(){return packages;}
    bool parse(QString filePath);
    void save();
    QString getFileLocation(){return fileLocation;}
    bool isInSubfolder(QString path);
    QSharedPointer<stringTableBase> findByName(QString name);
signals:

public slots:
private:
    QVector<QSharedPointer<stringTablePackage>> packages;
    QString fileLocation;
};







#endif // STRINGTABLEENTRY_H

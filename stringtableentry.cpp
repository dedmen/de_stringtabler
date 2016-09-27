#include "stringtableentry.h"
#include <QXmlStreamReader>
#include <QmessageBox>
#include <QFile>
#include <QDebug>

bool stringTableEntry::setTranslation(language lang, QString translation) {
    bool alreadyExisted = translations.contains(lang);
    translations[lang] = translation;
    return !alreadyExisted;
}

void stringTableEntry::clearTranslations(){translations.clear();}


void stringTablePackage::addContainer(QSharedPointer<stringTableContainer> &pContainer){
    if (!containers.contains(pContainer))
        containers.push_back(pContainer);
    pContainer->setPackage(sharedFromThis());
}

void stringTableContainer::addEntry(QSharedPointer<stringTableEntry> &pEntry)
{
    if (!entries.contains(pEntry))
        entries.push_back(pEntry);
    pEntry->setContainer(sharedFromThis());
}


void stringTable::addPackage(QSharedPointer<stringTablePackage> &pPackage)
{
    if (!packages.contains(pPackage))
        packages.push_back(pPackage);
    pPackage->setTable(sharedFromThis());
}
QString languageToString(language lang){
    switch (lang){
    case language::Original    : return "Original";
    case language::English     : return "English";
    case language::Czech       : return "Czech";
    case language::French      : return "French";
    case language::German      : return "German";
    case language::Italian     : return "Italian";
    case language::Polish      : return "Polish";
    case language::Portuguese  : return "Portuguese";
    case language::Russian     : return "Russian";
    case language::Spanish     : return "Spanish";
    case language::Korean      : return "Korean";
    case language::Japanese    : return "Japanese";
    case language::Turkish     : return "Turkish";
    }
}
language stringToLanguage(QStringRef lang){
    if (!lang.compare("Original",Qt::CaseInsensitive))
        return language::Original;
    else if (!lang.compare("English",Qt::CaseInsensitive))
        return language::English;
    else if (!lang.compare("Czech",Qt::CaseInsensitive))
        return language::Czech;
    else if (!lang.compare("French",Qt::CaseInsensitive))
        return language::French;
    else if (!lang.compare("German",Qt::CaseInsensitive))
        return language::German;
    else if (!lang.compare("Italian",Qt::CaseInsensitive))
        return language::Italian;
    else if (!lang.compare("Polish",Qt::CaseInsensitive))
        return language::Polish;
    else if (!lang.compare("Portuguese",Qt::CaseInsensitive))
        return language::Portuguese;
    else if (!lang.compare("Russian",Qt::CaseInsensitive))
        return language::Russian;
    else if (!lang.compare("Spanish",Qt::CaseInsensitive))
        return language::Spanish;
    else if (!lang.compare("Korean",Qt::CaseInsensitive))
        return language::Korean;
    else if (!lang.compare("Japanese",Qt::CaseInsensitive))
        return language::Japanese;
    else if (!lang.compare("Turkish",Qt::CaseInsensitive))
        return language::Turkish;
    else
        return language::invalid;
}
#define processTags(element) for (QXmlStreamAttribute attribute : xmlReader.attributes()){\
    if (!attribute.name().compare("name",Qt::CaseInsensitive)|| !attribute.name().compare("ID",Qt::CaseInsensitive)){\
    element->setName(attribute.value().toString());\
    } else\
    element->setTag(attribute.name().toString(),attribute.value().toString());\
    }

bool stringTable::parse(QString filePath)
{
    fileLocation = filePath;
    QFile inputFile(filePath);
    if (!inputFile.open(QFile::ReadOnly)){
        QMessageBox::warning(nullptr,"de_Stringtabler","Error encountered opening file \n"+filePath+"\nError: "+inputFile.errorString());
        return false;
    }
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&inputFile);

    while (!xmlReader.atEnd()) {
        xmlReader.readNextStartElement();

        while (!xmlReader.name().compare("Project",Qt::CaseInsensitive)){
            for (QXmlStreamAttribute attribute : xmlReader.attributes()){
                setTag(attribute.name().toString(),attribute.value().toString());
            }
            xmlReader.readNextStartElement();
            while (!xmlReader.name().compare("Package",Qt::CaseInsensitive)){
                QSharedPointer<stringTablePackage> newPackage = QSharedPointer<stringTablePackage>::create();

                for (QXmlStreamAttribute attribute : xmlReader.attributes()){
                    if (!attribute.name().compare("name",Qt::CaseInsensitive) || !attribute.name().compare("ID",Qt::CaseInsensitive)){
                        newPackage->setName(attribute.value().toString());
                    } else
                        newPackage->setTag(attribute.name().toString(),attribute.value().toString());
                }

                //processTags(newPackage);
                addPackage(newPackage);
                xmlReader.readNextStartElement();
                while (!xmlReader.name().compare("Container",Qt::CaseInsensitive)){
                    QSharedPointer<stringTableContainer> newContainer = QSharedPointer<stringTableContainer>::create();
                    processTags(newContainer);
                    newPackage->addContainer(newContainer);
                    xmlReader.readNextStartElement();
                    while (!xmlReader.name().compare("Key",Qt::CaseInsensitive)){
                        QSharedPointer<stringTableEntry> newEntry = QSharedPointer<stringTableEntry>::create();
                        processTags(newEntry);
                        newContainer->addEntry(newEntry);
                        xmlReader.readNextStartElement();
                        language lang = language::invalid;
                        while ((lang = stringToLanguage(xmlReader.name())) !=language::invalid ){
                            if (lang == language::invalid)
                                break;
                            QString translation = xmlReader.readElementText().trimmed();
                            newEntry->setTranslation(lang,translation);
                            xmlReader.readNextStartElement();
                        }
                        if (xmlReader.tokenType() != QXmlStreamReader::StartElement)//Sometimes doesnt read till next start element
                            xmlReader.readNextStartElement();
                    }
                    if (xmlReader.tokenType() != QXmlStreamReader::StartElement)//Sometimes doesnt read till next start element
                        xmlReader.readNextStartElement();
                }
                if (xmlReader.tokenType() != QXmlStreamReader::StartElement)//Sometimes doesnt read till next start element
                    xmlReader.readNextStartElement();

            }


        }
        xmlReader.readNext();
        qDebug() << xmlReader.name();


    }
    qDebug() << "parseDone";
    if (packages.empty()){
        QMessageBox::warning(nullptr,"de_Stringtabler","An error occured when parsing \n"+filePath+
                             "\nIf you think this file should contain valid data, do not save to this file!");
        return true;
    }
    inputFile.close();

    if (xmlReader.hasError())
        QMessageBox::warning(nullptr,"de_Stringtablerinputerino","Error parsing "+fileLocation+"\n"+xmlReader.errorString());
    else if (inputFile.error() != QFile::NoError)
        QMessageBox::warning(nullptr,"de_Stringtablerinputerino","Error parsing "+fileLocation+"\n"+inputFile.errorString());

    return true;




}

void stringTable::save()
{
    if (QMessageBox::question(nullptr,"de_Stringtablerino","Are you sure you want to save "+getName()+"?") != QMessageBox::Yes)
        return;


    QFile file(fileLocation);
    if (!file.open(QIODevice::WriteOnly)){

        QMessageBox::warning(nullptr,"de_Stringtablerinputerino","Error saving "+fileLocation+"\n"+file.errorString());


    }
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("Project");
    for (const QString & tagName : getTags().keys())
        xmlWriter.writeAttribute(tagName,getTags()[tagName]);

    for (QSharedPointer<stringTablePackage> & package : getPackages()){
        xmlWriter.writeStartElement("Package");
        xmlWriter.writeAttribute("name",package->getName());
        for (const QString & tagName : package->getTags().keys())
            xmlWriter.writeAttribute(tagName,package->getTags()[tagName]);
        for (QSharedPointer<stringTableContainer> & container : package->getContainers()){
            xmlWriter.writeStartElement("Container");
            xmlWriter.writeAttribute("name",container->getName());
            for (const QString & tagName : container->getTags().keys())
                xmlWriter.writeAttribute(tagName,container->getTags()[tagName]);
            for (QSharedPointer<stringTableEntry> & entry : container->getEntries()){
                xmlWriter.writeStartElement("Key");
                xmlWriter.writeAttribute("name",entry->getName());
                for (const QString & tagName : entry->getTags().keys())
                    xmlWriter.writeAttribute(tagName,entry->getTags()[tagName]);
                QMap<language,QString> translations = entry->getTranslations();
                for (language lang : translations.keys()){
                    if (lang >= language::invalid)
                        continue;
                    xmlWriter.writeStartElement(languageToString(lang));
                    xmlWriter.writeCharacters(translations[lang]);
                    xmlWriter.writeEndElement();
                    qDebug() << "wroteTranslation";
                }
                xmlWriter.writeEndElement();
            }
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file.close();
    qDebug() << "done";
}

bool stringTable::isInSubfolder(QString path)
{
    return path.startsWith(fileLocation.left(fileLocation.lastIndexOf("/")),Qt::CaseInsensitive);
}

QSharedPointer<stringTableBase> stringTable::findByName(QString name)
{
    for (QSharedPointer<stringTablePackage> & package : getPackages()){
        if (!package->getName().compare(name,Qt::CaseInsensitive))
            return package;
        for (QSharedPointer<stringTableContainer> & container : package->getContainers()){
            if (!container->getName().compare(name,Qt::CaseInsensitive))
                return container;
            for (QSharedPointer<stringTableEntry> & entry : container->getEntries()){
                if (!entry->getName().compare(name,Qt::CaseInsensitive))
                    return entry;
            }
        }
    }
    return QSharedPointer<stringTableBase>();
}

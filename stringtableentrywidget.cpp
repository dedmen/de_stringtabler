#include "stringtableentrywidget.h"
#include "ui_stringtableentrywidget.h"
#include <QDebug>
stringTableEntryWidget::stringTableEntryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::stringTableEntryWidget)
{
    ui->setupUi(this);
    highlighter = new stringSyntaxHighlighter(ui->text_usage->document());
    //TODO http://www.codeprogress.com/cpp/libraries/qt/showQtExample.php?index=185&key=QCompleterQLineEdit
}

stringTableEntryWidget::~stringTableEntryWidget()
{
    delete ui;
}

void stringTableEntryWidget::setCurrentItem(QSharedPointer<stringTableBase> newItem)
{
    if (newItem && newItem == currentEntry)
        return;
    entryType = 99;
    on_table_Translations_cellChanged(0,0);//forces save to
    ui->edit_EntryName->clear();
    ui->edit_EntryName->setDisabled(true);
    ui->edit_ContainerName->setDisabled(true);
    ui->edit_ContainerName->clear();
    ui->edit_PackageName->setDisabled(true);
    ui->edit_PackageName->clear();
    ui->table_Translations->clearContents();
    ui->table_Translations->setRowCount(0);
    ui->table_Translations->setDisabled(true);
    ui->table_tags->clearContents();
    ui->table_tags->setRowCount(0);
    ui->table_tags->setDisabled(true);
    ui->table_Translations->blockSignals(true);
    ui->table_tags->blockSignals(true);
    if (!newItem)
        return;

    currentEntry = newItem;
    QSharedPointer<stringTableEntry> entry = qSharedPointerDynamicCast<stringTableEntry>(currentEntry);
    QSharedPointer<stringTableContainer> container = qSharedPointerDynamicCast<stringTableContainer>(currentEntry);
    QSharedPointer<stringTablePackage> package = qSharedPointerDynamicCast<stringTablePackage>(currentEntry);
    if (entry){
        entryType = 0;
        ui->edit_EntryName->setDisabled(false);
        ui->edit_EntryName->setText(entry->getName());
        ui->edit_ContainerName->setDisabled(false);
        if (entry->getContainer())
            ui->edit_ContainerName->setText(entry->getContainer()->getName());
        else
            ui->edit_ContainerName->setText("");
        ui->edit_PackageName->setDisabled(false);
        if (entry->getContainer())
            ui->edit_PackageName->setText(entry->getContainer()->getPackage()->getName());
        else
            ui->edit_PackageName->setText(entry->getPackage()->getName());
        ui->table_Translations->setDisabled(false);
        QMap<language,QString> translations = entry->getTranslations();
        int row = 0;
        for(auto& it : translations.toStdMap())//#Performance toStdMap copies entire map http://stackoverflow.com/questions/8517853/iterating-over-a-qmap-with-for
        {
            ui->table_Translations->insertRow(ui->table_Translations->rowCount());
            QComboBox* combo = new QComboBox(NULL);
            combo->addItems({"Original", "English", "Czech", "French", "German", "Italian",
                             "Polish", "Portuguese", "Russian", "Spanish", "Korean", "Japanese", "Turkish", "Chinese", "Chineseimp","remove"});
            combo->setCurrentIndex((int)it.first);
            ui->table_Translations->setCellWidget(row,0,combo);
            ui->table_Translations->setItem(row,1,new QTableWidgetItem(it.second));

            row++;
        }

        ui->table_Translations->insertRow(ui->table_Translations->rowCount());

        QComboBox* combo = new QComboBox(NULL);
        combo->addItems({"Original", "English", "Czech", "French", "German", "Italian",
                         "Polish", "Portuguese", "Russian", "Spanish", "Korean", "Japanese", "Turkish", "Chinese", "Chineseimp","remove"});
        combo->setCurrentText("remove");
        ui->table_Translations->setCellWidget(row,0,combo);
        ui->table_Translations->setItem(row,1,new QTableWidgetItem());

        ui->list_Usages->clear();
        auto usages = entry->getUsages();
        for (fileLocation &usage : usages){
            QListWidgetItem* newItem = new QListWidgetItem(usage.filePath,ui->list_Usages);
            newItem->setData(Qt::UserRole,usage.offset);
            newItem->setData(Qt::UserRole +1 ,usage.endoffset);
        }
        ui->list_Usages->setCurrentRow(0);



    } else if (container){
        entryType = 1;
        ui->edit_EntryName->clear();

        ui->edit_ContainerName->setDisabled(false);
        ui->edit_ContainerName->setText(container->getName());
        ui->edit_PackageName->setDisabled(false);
        ui->edit_PackageName->setText(container->getPackage()->getName());
        ui->table_Translations->clear();

    } else if (package){
        entryType = 2;
        ui->edit_PackageName->setDisabled(false);
        ui->edit_PackageName->setText(package->getName());
    } else {
        return;
    }
    ui->table_tags->clear();
    ui->table_tags->setDisabled(false);
    int row = 0;
    for(auto& it : currentEntry->getTags().toStdMap())//#Performance toStdMap copies entire map http://stackoverflow.com/questions/8517853/iterating-over-a-qmap-with-for
    {
        qDebug() << it.first << it.second;
        ui->table_tags->insertRow(ui->table_tags->rowCount());
        ui->table_tags->setItem(row,0,new QTableWidgetItem(it.first));
        ui->table_tags->setItem(row,1,new QTableWidgetItem(it.second));

        row++;
    }
    ui->table_tags->insertRow(ui->table_tags->rowCount());
    ui->table_tags->setItem(row,0,new QTableWidgetItem());
    ui->table_tags->setItem(row,1,new QTableWidgetItem());

    ui->table_Translations->blockSignals(false);
    ui->table_tags->blockSignals(false);
}

void stringTableEntryWidget::setStringTables(QVector<QSharedPointer<stringTable> > pStringTables)
{
    stringTables = pStringTables;
    for (QSharedPointer<stringTable> & table : pStringTables){
        ui->combo_moveTo_stringtable->addItem(table->getName());
    }
}

void stringTableEntryWidget::on_edit_PackageName_returnPressed()
{
    //if package exists in table then move.. else create new package
    if (entryType == 0){//entry
        QSharedPointer<stringTableEntry> entry = qSharedPointerCast<stringTableEntry>(currentEntry);
        if (entry->getContainer())
            entry->getContainer()->getPackage()->setName(ui->edit_PackageName->text());
        else
            entry->getPackage()->setName(ui->edit_PackageName->text());
        emit updatedEntry(entry);
    } else if(entryType == 1){//container
        QSharedPointer<stringTableContainer> container = qSharedPointerCast<stringTableContainer>(currentEntry);
        QSharedPointer<stringTablePackage> package = container->getPackage();
        container->getPackage()->setName(ui->edit_PackageName->text());
        emit updatedEntry(container);
    } else if (entryType ==2){//package
        QSharedPointer<stringTablePackage> package = qSharedPointerCast<stringTablePackage>(currentEntry);
        package->setName(ui->edit_PackageName->text());
        emit updatedEntry(package);
    }
}

void stringTableEntryWidget::on_edit_ContainerName_returnPressed()
{
    //if container exists in table then move.. else create new container
    if (entryType == 0){//entry
        QSharedPointer<stringTableEntry> entry = qSharedPointerCast<stringTableEntry>(currentEntry);
        if (entry->getContainer()) //#TODO disable containerName field if entry is child of package
        entry->getContainer()->setName(ui->edit_ContainerName->text());
        emit updatedEntry(entry);
    } else if(entryType == 1){//container
        QSharedPointer<stringTableContainer> container = qSharedPointerCast<stringTableContainer>(currentEntry);
        QSharedPointer<stringTablePackage> package = container->getPackage();
        container->setName(ui->edit_ContainerName->text());
        emit updatedEntry(container);
    }
}

void stringTableEntryWidget::on_edit_EntryName_returnPressed()
{
    if (entryType != 0)
        return;
    currentEntry->setName(ui->edit_EntryName->text());
    emit updatedEntry(currentEntry);
}

void stringTableEntryWidget::on_combo_moveTo_stringtable_currentIndexChanged(const QString &arg1)
{
    ui->combo_moveTo_package->clear();
    if (entryType == 2)
        return;
    for(QSharedPointer<stringTable> &table : stringTables){
        if (!table->getName().compare(arg1)){
            for(QSharedPointer<stringTablePackage> &package : table->getPackages()){
                ui->combo_moveTo_package->addItem(package->getName());
            }
            return;
        }
    }
}

void stringTableEntryWidget::on_combo_moveTo_package_currentIndexChanged(const QString &arg1)
{
    ui->combo_moveTo_container->clear();
    if (entryType == 1)
        return;
    for(QSharedPointer<stringTable> &table : stringTables){
        if (!table->getName().compare(ui->combo_moveTo_stringtable->currentText())){
            for(QSharedPointer<stringTablePackage> &package : table->getPackages()){
                if (!package->getName().compare(arg1)){
                    for(QSharedPointer<stringTableContainer> &container : package->getContainers()){
                        ui->combo_moveTo_container->addItem(container->getName());
                    }
                    return;
                }
            }
            return;
        }
    }
}

void stringTableEntryWidget::on_button_moveTo_clicked()
{
    if (ui->combo_moveTo_container->currentText().isEmpty())
        return;
    QSharedPointer<stringTable> targetTable;
    QSharedPointer<stringTablePackage> targetPackage;
    QSharedPointer<stringTableContainer> targetContainer;


    for(QSharedPointer<stringTable> &table : stringTables){
        if (!table->getName().compare(ui->combo_moveTo_stringtable->currentText())){
            targetTable = table;
            if (entryType == 2)
                break;
            for(QSharedPointer<stringTablePackage> &package : table->getPackages()){
                if (!package->getName().compare(ui->combo_moveTo_package->currentText())){
                    targetPackage = package;
                    if (entryType == 1)
                        break;
                    for(QSharedPointer<stringTableContainer> &container : package->getContainers()){
                        if (!container->getName().compare(ui->combo_moveTo_container->currentText()))
                            targetContainer = container;
                    }
                    break;
                }
            }
            break;
        }
    }

    qDebug() << "move" << entryType << targetTable->getName() << targetPackage << targetContainer;

    if (!targetTable || (entryType < 2 && !targetPackage) || (entryType < 1 && !targetContainer))
        return;

    if (entryType == 0){//moving entry
        qDebug() << "move" << entryType << targetTable->getName()<< targetPackage->getName()<< targetContainer->getName();
        QSharedPointer<stringTableEntry> entry = qSharedPointerCast<stringTableEntry>(currentEntry);
        entry->getContainer()->removeEntry(entry);//#TODO fix for package with entry
        if (entry->getContainer()->getEntries().isEmpty()){
            QSharedPointer<stringTablePackage> package = entry->getContainer()->getPackage();
            package->removeContainer(entry->getContainer());
            if (package->getContainers().isEmpty())
                package->getTable()->removePackage(package);
        }
        entry->setContainer(targetContainer);
        targetContainer->addEntry(entry);
        qDebug() << "movedEntry";
        emit updatedEntry(entry);
    } else if(entryType == 1){//moving container
        QSharedPointer<stringTableContainer> container = qSharedPointerCast<stringTableContainer>(currentEntry);
        QSharedPointer<stringTablePackage> package = container->getPackage();
        package->removeContainer(container);
        if (package->getContainers().isEmpty())
            package->getTable()->removePackage(package);
        targetPackage->addContainer(container);
        emit updatedEntry(container);
    } else if (entryType ==2){//moving package
        QSharedPointer<stringTablePackage> package = qSharedPointerCast<stringTablePackage>(currentEntry);
        package->getTable()->removePackage(package);
        targetTable->addPackage(package);
        emit updatedEntry(package);
    }
}
#include <functional>
#include <QComboBox>
void stringTableEntryWidget::on_table_Translations_cellChanged(int row, int column)
{
    QSharedPointer<stringTableEntry> entry = qSharedPointerDynamicCast<stringTableEntry>(currentEntry);
    if (!entry)
        return;//#TODO throw error

    if (row == ui->table_Translations->rowCount()-1){
        if (((QComboBox*)ui->table_Translations->cellWidget(row,0))->currentText().compare("remove")){
            ui->table_Translations->insertRow(ui->table_Translations->rowCount());

            QComboBox* combo = new QComboBox(NULL);
            combo->addItems({"Original", "English", "Czech", "French", "German", "Italian",
                             "Polish", "Portuguese", "Russian", "Spanish", "Korean", "Japanese", "Turkish", "Chinese", "Chineseimp","remove"});
            combo->setCurrentText("remove");
            //TODO add slot that calls cellChanged when combo changed
            //connect( combo,&QComboBox::currentIndexChanged,this ,&stringTableEntryWidget::on_table_Translations_cellChanged);
            ui->table_Translations->setCellWidget(ui->table_Translations->rowCount()-1,0,combo);
            ui->table_Translations->setItem(ui->table_Translations->rowCount()-1,1,new QTableWidgetItem());
        }
    }
    entry->clearTranslations();
    for (int var = 0; var < ui->table_Translations->rowCount()-1; ++var) {
        if (!((QComboBox*)ui->table_Translations->cellWidget(var,0))->currentText().compare("remove")){
            continue;
        }
        language lang = (language)(((QComboBox*)ui->table_Translations->cellWidget(var,0))->currentIndex());
        QString translation = ui->table_Translations->item(var,1)->text();
        qDebug() << "set" << (int)lang << translation;
        entry->setTranslation(lang,translation);
    }
}
#include <QFile>
void stringTableEntryWidget::on_list_Usages_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    if (!current)
        return;
    QString filePath = current->text();
    int offset = current->data(Qt::UserRole).toInt();
    int offsetEnd = current->data(Qt::UserRole + 1).toInt();
    QFile usageFile(filePath);
    usageFile.open(QFile::ReadOnly);
    QString usageContent = QString::fromUtf8(usageFile.readAll());
    usageFile.close();
    ui->text_usage->setPlainText(usageContent);
    ui->text_usage->setReadOnly(true);
    auto cursor = ui->text_usage->textCursor();

    cursor.setPosition(offset -usageContent.leftRef(offset).count("\r\n"));//2 chars only need one char move
    cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,offsetEnd-offset);
    ui->text_usage->setTextCursor(cursor);
    ui->text_usage->ensureCursorVisible();
    ui->text_usage->setFocus();
    highlighter->setDocument(ui->text_usage->document());
}

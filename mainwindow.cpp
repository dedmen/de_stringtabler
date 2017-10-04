#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QDebug>
#include <QtConcurrent>
#include <functional>
#include "addentrydialog.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //http://www.qtcentre.org/threads/37753-Drag-and-Drop-items-in-QListWidget
    ui->widget_Entry->setCurrentItem(QSharedPointer<stringTableBase>());//lets widget disable itself
    connect(ui->widget_Entry,&stringTableEntryWidget::updatedEntry,this,&MainWindow::onEntryUpdated);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onEntryUpdated(QSharedPointer<stringTableBase>)
{
    on_tree_Tables_currentItemChanged(ui->tree_Tables->currentItem(),nullptr);
}

void MainWindow::on_actionLoadProject_triggered()
{
    QString directory = QFileDialog::getExistingDirectory(0,
                                                          "Give Project Path",
                                                          QString(),
                                                          QFileDialog::ShowDirsOnly);
if (directory.isEmpty()) return;

    currentSelectedEntry = QSharedPointer<stringTableBase>();
    ui->widget_Entry->setCurrentItem(QSharedPointer<stringTableBase>());
    ui->widget_Entry->setStringTables(QVector<QSharedPointer<stringTable>>());
    ui->tree_Entries->clear();
    ui->tree_Tables->clear();
    stringTables.clear();
    QDirIterator it(directory, QStringList() << "stringtable.xml", QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    QVector<QSharedPointer<stringTable>> tablesVector;
    while (it.hasNext()){
        auto tablePath = it.next();
        QSharedPointer<stringTable> table = QSharedPointer<stringTable>::create();
        table->setName(tablePath.right(
                           tablePath.length()-tablePath.lastIndexOf("/", -(tablePath.length()-tablePath.lastIndexOf("/") +1))
                           -1));
        if (table->parse(tablePath)){
            stringTables[table->getName()] = table;
            tablesVector.push_back(table);
        }
        qDebug() << "table" << table->getName() << tablePath;
    }
    ui->widget_Entry->setStringTables(tablesVector);
    QMap<QString,QSharedPointer<stringTable>> tables;
    for (const QString & it : stringTables.keys()){
        tables[it.left(it.indexOf("/"))] = stringTables[it];
    }



    for (const QString & it : tables.keys()){

        QTreeWidgetItem* topItem = new QTreeWidgetItem();
        topItem->setText(0,it);
        ui->tree_Tables->addTopLevelItem(topItem);


        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0,"stringtable.xml");
        item->setData(0,Qt::UserRole,tables[it]->getName());
        topItem->addChild(item);
    }
    ui->tree_Tables->expandAll();
    qDebug() << "done";

    cConflictFinder.setStringTables(tablesVector);
    cConflictFinder.setBasePath(directory);
    QtConcurrent::run(std::bind(&stringtableConflictFinder::run,&cConflictFinder));
}

void MainWindow::on_tree_Tables_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if (!current || !current->parent())
        return;



    if (!stringTables.contains(current->parent()->text(0)+"/"+current->text(0)))
        return;
    auto table = stringTables.value(current->parent()->text(0)+"/"+current->text(0));

    ui->tree_Entries->clear();



    QVector<QSharedPointer<stringTablePackage>> packages = table->getPackages();


    for (QSharedPointer<stringTablePackage> &package: packages){
        auto packageItem = new QTreeWidgetItem();
        packageItem->setText(0,package->getName());
        packageItem->setData(0,Qt::UserRole,2);
        QVector<QSharedPointer<stringTableContainer>> containers =  package->getContainers();
        for (QSharedPointer<stringTableContainer> &container: containers){
            auto containerItem = new QTreeWidgetItem();
            containerItem->setText(0,container->getName());
            containerItem->setData(0,Qt::UserRole,1);
            QVector<QSharedPointer<stringTableEntry>> entries =  container->getEntries();
            for (QSharedPointer<stringTableEntry> &entry: entries){
                if (!ui->edit_filter->text().isEmpty()){
                    bool filterMatch = entry->getName().contains(ui->edit_filter->text(),Qt::CaseInsensitive);
                    if (!filterMatch)
                        for (QString& it : entry->getTranslations().values())
                            if (it.contains(ui->edit_filter->text(),Qt::CaseInsensitive))
                            {
                                filterMatch = true;
                                break;
                            }
                    if (!filterMatch)
                        for (QString& it : entry->getTags().values())
                            if (it.contains(ui->edit_filter->text(),Qt::CaseInsensitive))
                            {
                                filterMatch = true;
                                break;
                            }
                    if (!filterMatch)
                        continue;
                }

                auto entryItem = new QTreeWidgetItem();
                entryItem->setText(0,entry->getName());
                entryItem->setData(0,Qt::UserRole,0);
                containerItem->addChild(entryItem);
            }
            packageItem->addChild(containerItem);
        }






        QVector<QSharedPointer<stringTableEntry>> entries =  package->getEntries();
        for (QSharedPointer<stringTableEntry> &entry: entries){
            if (!ui->edit_filter->text().isEmpty()){
                bool filterMatch = entry->getName().contains(ui->edit_filter->text(),Qt::CaseInsensitive);
                if (!filterMatch)
                    for (QString& it : entry->getTranslations().values())
                        if (it.contains(ui->edit_filter->text(),Qt::CaseInsensitive))
                        {
                            filterMatch = true;
                            break;
                        }
                if (!filterMatch)
                    for (QString& it : entry->getTags().values())
                        if (it.contains(ui->edit_filter->text(),Qt::CaseInsensitive))
                        {
                            filterMatch = true;
                            break;
                        }
                if (!filterMatch)
                    continue;
            }

            auto entryItem = new QTreeWidgetItem();
            entryItem->setText(0,entry->getName());
            entryItem->setData(0,Qt::UserRole,0);
            packageItem->addChild(entryItem);
        }














        ui->tree_Entries->addTopLevelItem(packageItem);
    }
    ui->tree_Entries->expandAll();


}

void MainWindow::on_tree_Entries_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if (!current)
        return;
    QSharedPointer<stringTable> table =  stringTables.value(ui->tree_Tables->currentItem()->data(0,Qt::UserRole).toString());
    QMap<QString,QSharedPointer<stringTablePackage>> packages;
    for (QSharedPointer<stringTablePackage>& package : table->getPackages()){
        packages[package->getName()] = package;
    }

    switch (current->data(0,Qt::UserRole).toInt()){
    case 0://entry
    {
        if (current->parent()->parent()){//Has container and package
            QSharedPointer<stringTablePackage> package = packages[current->parent()->parent()->text(0)];
            for (QSharedPointer<stringTableContainer>& container : package->getContainers()){
                if (!container->getName().compare(current->parent()->text(0))){
                    for (QSharedPointer<stringTableEntry>& entry : container->getEntries()){
                        if (!entry->getName().compare(current->text(0))){
                            ui->widget_Entry->setCurrentItem(entry);
                            currentSelectedEntry = entry;
                            return;
                        }
                    }
                }
            }
        }
 QSharedPointer<stringTablePackage> package = packages[current->parent()->text(0)];
//No container. Direct child of package
        for (QSharedPointer<stringTableEntry>& entry : package->getEntries()){
            if (!entry->getName().compare(current->text(0))){
                ui->widget_Entry->setCurrentItem(entry);
                currentSelectedEntry = entry;
                return;
            }
        }
        break;
    }
    case 1://container
    {
        QSharedPointer<stringTablePackage> package = packages[current->parent()->text(0)];
        for (QSharedPointer<stringTableContainer>& container : package->getContainers()){
            if (!container->getName().compare(current->text(0))){
                ui->widget_Entry->setCurrentItem(container);
                currentSelectedEntry = container;
                return;
            }
        }
        break;
    }
    case 2://package
        ui->widget_Entry->setCurrentItem(packages[current->text(0)]);
        currentSelectedEntry = packages[current->text(0)];
        break;
}
}

void MainWindow::on_edit_filter_textChanged(const QString &arg1)
{
    on_tree_Tables_currentItemChanged(ui->tree_Tables->currentItem(),nullptr);
}

void MainWindow::on_actionAdd_Something_triggered()
{
    auto dia= new addEntryDialog(this);
    dia->setStringTables(stringTables.values().toVector());
    dia->setCurrentSelectedEntry(currentSelectedEntry);
    connect(dia,&addEntryDialog::addedEntry,this,&MainWindow::onEntryUpdated);
    dia->show();
}

void MainWindow::on_actionSaveProject_triggered()
{
    for (auto &it : stringTables)
        it->save();
}

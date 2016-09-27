#include "addentrydialog.h"
#include "ui_addentrydialog.h"

addEntryDialog::addEntryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addEntryDialog)
{
    ui->setupUi(this);
}

addEntryDialog::~addEntryDialog()
{
    delete ui;
}

void addEntryDialog::setStringTables(QVector<QSharedPointer<stringTable> > pStringTables)
{
    stringTables = pStringTables;
    ui->combo_Container_Stringtable->blockSignals(true);


    ui->combo_Key_Stringtable->blockSignals(true);
    ui->combo_Package_Stringtable->blockSignals(true);
    ui->combo_Key_Stringtable->addItem("");
    ui->combo_Container_Stringtable->addItem("");
    ui->combo_Package_Stringtable->addItem("");

    for (QSharedPointer<stringTable> &stringTable: stringTables){
        ui->combo_Key_Stringtable->addItem(stringTable->getName());
        ui->combo_Container_Stringtable->addItem(stringTable->getName());
        ui->combo_Package_Stringtable->addItem(stringTable->getName());
    }

    ui->combo_Key_Stringtable->blockSignals(false);
    ui->combo_Container_Stringtable->blockSignals(false);

    ui->combo_Package_Stringtable->blockSignals(false);




}

void addEntryDialog::setCurrentSelectedEntry(QSharedPointer<stringTableBase>)
{
    //#TODO preselect stuff
}

void addEntryDialog::on_buttonBox_accepted()
{
    if (ui->tabWidget->currentIndex() == 0){//key
        QSharedPointer<stringTableEntry> newEntry = QSharedPointer<stringTableEntry>::create();
        for (QSharedPointer<stringTable> &stringTable: stringTables){
            if (stringTable->getName().compare(ui->combo_Key_Stringtable->currentText()))
                continue;
            for (QSharedPointer<stringTablePackage> &package: stringTable->getPackages()){
                if (package->getName().compare(ui->combo_key_Package->currentText()))
                    continue;
                QVector<QSharedPointer<stringTableContainer>> containers =  package->getContainers();
                for (QSharedPointer<stringTableContainer> &container: containers){
                    if (container->getName().compare(ui->combo_key_Container->currentText()))
                        continue;
                    newEntry->setContainer(container);
                    container->addEntry(newEntry);
                    emit addedEntry(newEntry);
                    return;
                }
            }
        }


    } else if (ui->tabWidget->currentIndex() == 1){//container
        QSharedPointer<stringTableContainer> newContainer = QSharedPointer<stringTableContainer>::create();
        newContainer->setName(ui->edit_Container_ContainerName->text());
        for (QSharedPointer<stringTable> &stringTable: stringTables){
            if (stringTable->getName().compare(ui->combo_Container_Stringtable->currentText()))
                continue;
            for (QSharedPointer<stringTablePackage> &package: stringTable->getPackages()){
                if (package->getName().compare(ui->combo_Container_Package->currentText()))
                    continue;
                newContainer->setPackage(package);
                package->addContainer(newContainer);
                emit addedEntry(newContainer);
                return;
            }
        }

    } else if (ui->tabWidget->currentIndex() == 2){//package
        QSharedPointer<stringTablePackage> newPackage = QSharedPointer<stringTablePackage>::create();
        newPackage->setName(ui->edit_Package_PackageName->text());

        for (QSharedPointer<stringTable> &stringTable: stringTables){
            if (stringTable->getName().compare(ui->combo_Key_Stringtable->currentText()))
                continue;
            newPackage->setTable(stringTable);
            stringTable->addPackage(newPackage);
            emit addedEntry(newPackage);
            return;
        }
    }

}

void addEntryDialog::stringTable_selected(const QString &arg1)
{
    QStringList packages;
    for (QSharedPointer<stringTable> &stringTable: stringTables){
        if (stringTable->getName().compare(arg1))
            continue;
        for (QSharedPointer<stringTablePackage> &package: stringTable->getPackages()){
            packages << package->getName();

        }
    }
    ui->combo_Container_Package->blockSignals(true);
    ui->combo_key_Package->blockSignals(true);
    ui->combo_Container_Package->addItem("");
    ui->combo_key_Package->addItem("");
    ui->combo_Container_Package->addItems(packages);
    ui->combo_key_Package->addItems(packages);
    ui->combo_Container_Package->blockSignals(false);
    ui->combo_key_Package->blockSignals(false);
}

void addEntryDialog::package_selected(const QString &stringtableName, const QString &packageName)
{
    QStringList containers;
    for (QSharedPointer<stringTable> &stringTable: stringTables){
        if (stringTable->getName().compare(stringtableName))
            continue;
        for (QSharedPointer<stringTablePackage> &package: stringTable->getPackages()){
            if (package->getName().compare(packageName))
                continue;
            for (QSharedPointer<stringTableContainer> &container: package->getContainers()){
                containers << container->getName();
            }
        }
    }
    ui->combo_key_Container->blockSignals(true);
    ui->combo_key_Container->addItem("");
    ui->combo_key_Container->addItems(containers);
    ui->combo_key_Container->blockSignals(false);
}

void addEntryDialog::on_combo_Package_Stringtable_currentTextChanged(const QString &arg1)
{
    stringTable_selected(arg1);
    ui->combo_Container_Stringtable->blockSignals(true);
    ui->combo_Container_Stringtable->setCurrentText(arg1);
    ui->combo_Container_Stringtable->blockSignals(false);
    ui->combo_Key_Stringtable->blockSignals(true);
    ui->combo_Key_Stringtable->setCurrentText(arg1);
    ui->combo_Key_Stringtable->blockSignals(false);
}

void addEntryDialog::on_combo_Container_Stringtable_currentTextChanged(const QString &arg1)
{
    stringTable_selected(arg1);
    ui->combo_Package_Stringtable->blockSignals(true);
    ui->combo_Package_Stringtable->setCurrentText(arg1);
    ui->combo_Package_Stringtable->blockSignals(false);
    ui->combo_Key_Stringtable->blockSignals(true);
    ui->combo_Key_Stringtable->setCurrentText(arg1);
    ui->combo_Key_Stringtable->blockSignals(false);
}

void addEntryDialog::on_combo_Key_Stringtable_currentTextChanged(const QString &arg1)
{
    stringTable_selected(arg1);
    ui->combo_Container_Stringtable->blockSignals(true);
    ui->combo_Container_Stringtable->setCurrentText(arg1);
    ui->combo_Container_Stringtable->blockSignals(false);
    ui->combo_Package_Stringtable->blockSignals(true);
    ui->combo_Package_Stringtable->setCurrentText(arg1);
    ui->combo_Package_Stringtable->blockSignals(false);
}

void addEntryDialog::on_combo_key_Package_currentTextChanged(const QString &arg1)
{
    package_selected(ui->combo_Key_Stringtable->currentText(),arg1);
    ui->combo_Container_Package->blockSignals(true);
    ui->combo_Container_Package->setCurrentText(arg1);
    ui->combo_Container_Package->blockSignals(false);
}

void addEntryDialog::on_combo_Container_Package_currentTextChanged(const QString &arg1)
{
    package_selected(ui->combo_Key_Stringtable->currentText(),arg1);
    ui->combo_key_Package->blockSignals(true);
    ui->combo_key_Package->setCurrentText(arg1);
    ui->combo_key_Package->blockSignals(false);
}

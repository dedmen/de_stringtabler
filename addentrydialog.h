#ifndef ADDENTRYDIALOG_H
#define ADDENTRYDIALOG_H

#include <QDialog>
#include "stringtableentry.h"
namespace Ui {
class addEntryDialog;
}

class addEntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addEntryDialog(QWidget *parent = 0);
    ~addEntryDialog();
    void setStringTables(QVector<QSharedPointer<stringTable>> pStringTables);
    void setCurrentSelectedEntry(QSharedPointer<stringTableBase>);
signals:
    void addedEntry(QSharedPointer<stringTableBase>);
private slots:
    void on_buttonBox_accepted();

    void stringTable_selected(const QString &arg1);
    void package_selected(const QString &stringtable,const QString &package);
    void on_combo_Package_Stringtable_currentTextChanged(const QString &arg1);

    void on_combo_Container_Stringtable_currentTextChanged(const QString &arg1);

    void on_combo_Key_Stringtable_currentTextChanged(const QString &arg1);

    void on_combo_key_Package_currentTextChanged(const QString &arg1);

    void on_combo_Container_Package_currentTextChanged(const QString &arg1);

private:
    Ui::addEntryDialog *ui;
    QVector<QSharedPointer<stringTable>> stringTables;
};

#endif // ADDENTRYDIALOG_H

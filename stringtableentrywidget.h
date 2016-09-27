#ifndef STRINGTABLEENTRYWIDGET_H
#define STRINGTABLEENTRYWIDGET_H

#include <QWidget>
#include "stringtableentry.h"
#include <QListWidgetItem>
#include "stringsyntaxhighlighter.h"
namespace Ui {
class stringTableEntryWidget;
}

class stringTableEntryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit stringTableEntryWidget(QWidget *parent = 0);
    ~stringTableEntryWidget();
void setCurrentItem(QSharedPointer<stringTableBase> newItem);
void setStringTables(QVector<QSharedPointer<stringTable>> pStringTables);

signals:
        void updatedEntry(QSharedPointer<stringTableBase>);
private slots:
        void on_edit_PackageName_returnPressed();

        void on_edit_ContainerName_returnPressed();

        void on_edit_EntryName_returnPressed();

        void on_combo_moveTo_stringtable_currentIndexChanged(const QString &arg1);

        void on_combo_moveTo_package_currentIndexChanged(const QString &arg1);

        void on_button_moveTo_clicked();

        void on_table_Translations_cellChanged(int row, int column);
        void on_list_Usages_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    QSharedPointer<stringTableBase> currentEntry;
    int entryType;//0 entry, 1 container, 2 package
    QVector<QSharedPointer<stringTable>> stringTables;
    Ui::stringTableEntryWidget *ui;
    stringSyntaxHighlighter *highlighter;
};

#endif // STRINGTABLEENTRYWIDGET_H

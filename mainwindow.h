#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stringtableentry.h"
#include <QTreeWidget>
#include "stringtableconflictfinder.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void onEntryUpdated(QSharedPointer<stringTableBase>);
private slots:
    void on_actionLoadProject_triggered();

    void on_tree_Tables_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_tree_Entries_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_edit_filter_textChanged(const QString &arg1);
    void on_actionAdd_Something_triggered();

    void on_actionSaveProject_triggered();

private:
    stringtableConflictFinder cConflictFinder;
    Ui::MainWindow *ui;
   QSharedPointer<stringTableBase> currentSelectedEntry;
    QMap<QString,QSharedPointer<stringTable>> stringTables;
};

#endif // MAINWINDOW_H

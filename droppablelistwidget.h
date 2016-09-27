#ifndef DROPPABLELISTWIDGET_H
#define DROPPABLELISTWIDGET_H

#include <QListWidget>
class DroppableListWidget : public QListWidget
{
public:
    DroppableListWidget(QWidget *parent = 0);
protected:
    void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dragLeaveEvent(QDragLeaveEvent *event);
        void dropEvent(QDropEvent *event);
};

#endif // DROPPABLELISTWIDGET_H

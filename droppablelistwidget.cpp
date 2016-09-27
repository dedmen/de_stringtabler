#include "droppablelistwidget.h"
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
DroppableListWidget::DroppableListWidget(QWidget *parent) : QListWidget(parent)
{

}

void DroppableListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "enter";
    event->acceptProposedAction();
}

void DroppableListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << "move";
    event->acceptProposedAction();
}

void DroppableListWidget::dropEvent(QDropEvent *event) {
    qDebug() << "drop" << event->mimeData()->text() << event->mimeData()->formats();
    const QMimeData *mimeData = event->mimeData();
    for(int i = 0; i < count(); ++i)
    {
        //QListWidgetItem* item = item(i);
        //Do stuff!
    }
    //itemAt(event->pos());//Emit signal with item and ptr to data from event


    if (mimeData->hasUrls()) {
       //Insert the URLs and set flags to Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    }
    event->acceptProposedAction();
}

void DroppableListWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    qDebug() << "leave";
    event->accept();
}

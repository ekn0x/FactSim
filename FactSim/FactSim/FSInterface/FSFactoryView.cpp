#include "FSFactoryView.h"

#include <QMouseEvent>

FS::FactoryView::FactoryView(QGraphicsScene * scene, QWidget * parent)
	: QInteractiveGraphicsView(scene)
{

}

void FS::FactoryView::mouseReleaseEvent(QMouseEvent *event)
{
	if (itemAt(event->pos()))
	{
		emit activeObject(itemAt(event->pos()));
	}
	else
	{
		qDebug("You didn't click on an item.");
		emit activeObject(nullptr);
	}
}
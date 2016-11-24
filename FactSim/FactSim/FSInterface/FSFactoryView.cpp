#include "FSFactoryView.h"

#include <QMouseEvent>

FS::FactoryView::FactoryView(QGraphicsScene * scene, QWidget * parent)
	: QInteractiveGraphicsView(scene)
{

}

void FS::FactoryView::mouseReleaseEvent(QMouseEvent *event)
{
	if (itemAt(event->pos()))
		emit activeObject(itemAt(event->pos()));
	else
		emit activeObject(nullptr);
}
#include "FSWorkspace.h"

#include <QPainter>

FS::Workspace::Workspace(int XPos, int YPos, int Width, int Height)
	: mXPos{ XPos }, mYPos{ YPos }, mWidth{ Width }, mHeight{ Height }
{

}

QRectF FS::Workspace::boundingRect() const
{
	qreal penWidth = 5;
	return QRectF(mXPos, mYPos, 20 + penWidth, 20 + penWidth);
}

void FS::Workspace::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->drawRect(mXPos, mYPos, 20, 20);
}
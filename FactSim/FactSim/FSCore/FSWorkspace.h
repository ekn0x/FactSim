#ifndef FS_WORKSPACE_H
#define FS_WORKSPACE_H

#include "FSMachine.h"

namespace FS
{
	class Workspace : public Machine
	{
	public:
		Workspace() : Workspace(0, 0, 20, 20) {};
		Workspace(int XPos, int YPos) : Workspace(XPos, YPos, 20, 20) {};
		Workspace(int XPos, int YPos, int Width, int Height);
		~Workspace() = default;

		virtual QRectF boundingRect() const override;
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	protected:
		int mXPos;
		int mYPos;
		int mWidth;
		int mHeight;
	};
};

#endif // FS_WORKSPACE_H
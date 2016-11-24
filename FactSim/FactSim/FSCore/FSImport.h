#ifndef FS_IMPORT_H
#define FS_IMPORT_H

#include "FSWorkspace.h"

namespace FS
{
	class Import : public Workspace
	{
	public:
		Import() = default;
		~Import() = default;

		virtual QRectF boundingRect() const override;
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	private:

	};

};

#endif // FS_IMPORT_H

#ifndef FS_FACTORYVIEW_H
#define FS_FACTORYVIEW_H

#include "Provided\QInteractiveGraphicsView.h"


namespace FS
{
	class FactoryView : public QInteractiveGraphicsView
	{
		Q_OBJECT

	public:
		FactoryView(QGraphicsScene * scene, QWidget * parent = nullptr);
		~FactoryView() = default;

	signals:
		void activeObject(QGraphicsItem *tgt);

	protected:
		virtual void mouseReleaseEvent(QMouseEvent *event) override;

	private:

	};
};

#endif // FS_FACTORYVIEW_H

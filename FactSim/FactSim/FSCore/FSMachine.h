#ifndef FS_MACHINE_H
#define FS_MACHINE_H

#include <QGraphicsItem>
#include <QtMath>

namespace FS
{
	class Machine : public QGraphicsItem
	{
	public:
		Machine() = default;
		virtual ~Machine() = default;

		void setSpeed(qreal s);
		void setName(QString n);
		void setDescription(QString d);

		qreal speed() { return mSpeed; }
		QString name() { return mName; }
		QString description() { return mDescription; }

	private:
		qreal mSpeed;
		QString mName;
		QString mDescription;
	};
};

#endif // FS_MACHINE_H

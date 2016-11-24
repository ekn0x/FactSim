#ifndef FS_MACHINE_INFORMATION_H
#define FS_MACHINE_INFORMATION_H

#include <QWidget>

class QLabel;
class QGroupBox;
class QGraphicsItem;

namespace FS
{
	class Machine;

	class MachineInformation : public QWidget
	{
		Q_OBJECT

	public slots:
		void activeObject(QGraphicsItem *tgt);

	public:
		MachineInformation(QWidget *parent = nullptr);
		~MachineInformation() = default;

	private:
		QLabel *mName;
		QLabel *mType;
		QLabel *mDescription;
	};
};

#endif // FS_MACHINE_INFORMATION_H

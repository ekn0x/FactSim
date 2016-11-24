#ifndef FS_MACHINE_PARAMETERS_H
#define FS_MACHINE_PARAMETERS_H

#include <QWidget>

class QLabel;
class QSlider;
class QGroupBox;
class QGraphicsItem;

namespace FS
{
	class Machine;

	class MachineParameters : public QWidget
	{
		Q_OBJECT

	public slots:
		void activeObject(QGraphicsItem *tgt);

	public:
		MachineParameters(QWidget *parent = nullptr);
		~MachineParameters() = default;

	private:
		QSlider *mSpeed;
	};
};

#endif // FS_MACHINE_PARAMETERS_H

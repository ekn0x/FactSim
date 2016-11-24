#ifndef FS_INTERFACE_H
#define FS_INTERFACE_H

#include <QWidget>
#include <QElapsedTimer>

class QGraphicsItem;
class QGraphicsScene;
class QPushButton;

class QTimer;

namespace FS
{
	class FactoryView;
	class MachineInformation;
	class MachineParameters;
	class MachineStatistics;
	class FactSimStats;

	class Interface : public QWidget
	{
		Q_OBJECT

	public:
		Interface(QWidget *parent = nullptr);
		~Interface() = default;

		// This function is specific to this demo since, the scene is hardcoded within the programm
		// otherwise there would a builder function where the user can add building blocks and shiits
		void buildDemoScene();

	public slots:
		void tick();

	private:
		// side panel
		QPushButton *mPower;
		FS::MachineInformation *mMachineInfo;
		FS::MachineParameters *mMachineParam;
		FS::MachineStatistics *mMachineStats;
		FS::FactSimStats *mSimStats;

		// World timer
		QTimer *mTimer;
		QElapsedTimer mElapsedTimer;

		// main panel
		FS::FactoryView *mView;
		QGraphicsScene *mScene;
	};
};

#endif // FS_INTERFACE_H

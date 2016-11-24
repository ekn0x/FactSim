#ifndef FS_MACHINESTATISTICS_H
#define FS_MACHINESTATISTICS_H

#include <QWidget>

namespace FS
{

	class MachineStatistics : public QWidget
	{
		Q_OBJECT

	public:
		MachineStatistics(QWidget *parent = nullptr);
		~MachineStatistics() = default;

	private:
		
	};
};

#endif //FS_MACHINESTATISTICS_H

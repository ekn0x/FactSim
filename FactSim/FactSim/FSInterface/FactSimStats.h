#ifndef FS_FACTSIMSTATS_H
#define FS_FACTSIMSTATS_H

#include <QWidget>

class QLabel;

namespace FS
{
	class FactSimStats : public QWidget
	{
		Q_OBJECT

	public:
		FactSimStats(QWidget *parent = nullptr);
		~FactSimStats() = default;
	
		void setFPS(qreal fps);

	private:
		QLabel *mFPS;
	};
};

#endif // FS_FACTSIMSTATS_H

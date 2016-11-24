#include "FactSimStats.h"

#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>

FS::FactSimStats::FactSimStats(QWidget *parent)
{
	mFPS = new QLabel(QString("FPS : "));
	mFPS->setAlignment(Qt::AlignRight);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(mFPS);
	
	QGroupBox *gb = new QGroupBox(QString("Simulation statistics"));
	gb->setLayout(layout);

	QGridLayout *f = new QGridLayout;
	f->addWidget(gb, 0, 0);
	setLayout(f);
}

void FS::FactSimStats::setFPS(qreal fps)
{
	mFPS->setText(QString("FPS : %1").arg(fps));
}
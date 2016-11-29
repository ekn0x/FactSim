#include "MachineParameters.h"

#include <QLabel>
#include <qslider.h>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QGraphicsItem>

#include "FSCore\FSMachine.h"
#include "FSCore\FSImport.h"

FS::MachineParameters::MachineParameters(QWidget *parent)
{
	QGroupBox *GroupBox = new QGroupBox;
	GroupBox->setTitle(QString("Machine's Parameters"));

	// machine speed
	QLabel * speedLabel = new QLabel(QString("Speed"));
	speedLabel->setFixedWidth(150);
	speedLabel->setFixedHeight(50);

	mSpeed = new QSlider;
	mSpeed->setMinimum(0);
	mSpeed->setMaximum(100);
	mSpeed->setFixedWidth(150);
	mSpeed->setOrientation(Qt::Horizontal);

	// grouping in one box
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(speedLabel);
	layout->addWidget(mSpeed);
	GroupBox->setLayout(layout);

	// setting the final layout
	QGridLayout *tmp = new QGridLayout;
	tmp->addWidget(GroupBox, 0, 0);
	setLayout(tmp);
}

void FS::MachineParameters::activeObject(QGraphicsItem *tgt)
{
	if (!tgt)
	{
		mSpeed->setValue(0);
		return;
	}

	if (dynamic_cast<FS::Workspace*>(tgt))
	{
		if (dynamic_cast<FS::Import*>(tgt))
		{
			FS::Import* castedTgt = dynamic_cast<FS::Import*>(tgt);
			mSpeed->setValue(static_cast<int>(castedTgt->speed()));
		}/*
		 else if (dynamic_cast<FS::Export*>(tgt))
		 {

		 }
		 else if (dynamic_cast<FS::Transform*>(tgt))
		 {

		 }*/
		/*else // generic machine
		{
			FS::Machine* castedTgt = dynamic_cast<FS::Machine*>(tgt);
			mSpeed->setValue(castedTgt->speed());
		}*/

	}/*
	 else if (dynamic_cast<FS::Transporter*>(tgt))
	 {

	 }
	 else if (dynamic_cast<FS::Material*>(tgt))
	 {

	 }*/
	else // unknown object
	{
		mSpeed->setValue(0);
	}
}

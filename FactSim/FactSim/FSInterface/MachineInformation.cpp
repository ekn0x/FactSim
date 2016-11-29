#include "MachineInformation.h"

#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QGraphicsItem>

#include "FSCore\FSMachine.h"
#include "FSCore\FSImport.h"

FS::MachineInformation::MachineInformation(QWidget *parent)
{
	QGroupBox *GroupBox = new QGroupBox;
	GroupBox->setTitle(QString("Machine's informations"));
	// machine name
	mName = new QLabel(QString("Machine's Name"));
	mName->setFixedWidth(150);
	mName->setFixedHeight(15);

	// machine type
	mType = new QLabel(QString("Machine's type"));
	mType->setFixedWidth(150);
	mType->setFixedHeight(15);

	// machine description
	mDescription = new QLabel(QString("Machine's Description"));
	mDescription->setFixedWidth(150);
	mDescription->setFixedHeight(50);
	mDescription->setWordWrap(true);

	// grouping in one box
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(mName);
	layout->addWidget(mType);
	layout->addWidget(mDescription);
	layout->addStretch();
	GroupBox->setLayout(layout);

	// setting the final layout
	QGridLayout *tmp = new QGridLayout;
	tmp->addWidget(GroupBox, 0, 0);
	setLayout(tmp);
}

void FS::MachineInformation::activeObject(QGraphicsItem *tgt)
{
	if (tgt == nullptr)
	{
		mName->setText(QString("Machine's name"));
		mType->setText(QString("Machine's type"));
		mDescription->setText(QString("Machine's description"));
		return;
	}

	if (dynamic_cast<FS::Workspace*>(tgt))
	{
		if (dynamic_cast<FS::Import*>(tgt))
		{
			FS::Import* castedTgt = dynamic_cast<FS::Import*>(tgt);
			mName->setText(castedTgt->name());
			mType->setText("Material Import");
			mDescription->setText(castedTgt->description());
		}/*
		else if (dynamic_cast<FS::Export*>(tgt))
		{

		}
		else if (dynamic_cast<FS::Transform*>(tgt))
		{

		}*/
		else // generic machine
		{
			FS::Machine* castedTgt = dynamic_cast<FS::Machine*>(tgt);
			mName->setText(castedTgt->name());
			mType->setText("Generic machine");
			mDescription->setText(castedTgt->description());
		}
		
	}/*
	else if (dynamic_cast<FS::Transporter*>(tgt))
	{

	}
	else if (dynamic_cast<FS::Material*>(tgt))
	{

	}*/
	else // unknown object
	{
		mName->setText("Unknown object");
		mType->setText("Unknown object");
		mDescription->setText("Unknown object");
	}


	
}

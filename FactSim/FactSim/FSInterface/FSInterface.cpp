#include "FSInterface.h"

#include <QGraphicsScene>

#include <QPushButton>
#include <QTimer>

#include <QHBoxLayout>
#include <QVBoxLayout>

// Factory Simulator interface packages
#include "FSFactoryView.h"
#include "FSFactoryScene.h"
#include "MachineInformation.h"
#include "MachineParameters.h"
#include "FactSimStats.h"
#include "Provided\QInteractiveGraphicsView.h"

// Machine package
#include "FSCore\FSMachine.h"
#include "FSCore\FSImport.h"

FS::Interface::Interface(QWidget *parent)
{
	// setting timer
	mTimer = new QTimer(this);
	connect(mTimer, &QTimer::timeout, this, &FS::Interface::tick);
	mTimer->start(15);
	mElapsedTimer.start();

	// set default power button
	mPower = new QPushButton(QString("Power On"));
	mPower->setFixedWidth(200);

	// set default machine informations
	mMachineInfo = new FS::MachineInformation;
	mMachineInfo->setFixedWidth(200);

	// set default machine parameters
	mMachineParam = new FS::MachineParameters;
	mMachineParam->setFixedWidth(200);

	// set default machine statistics informations

	// Simulation statistics
	mSimStats = new FS::FactSimStats;
	mSimStats->setFixedWidth(200);

	// set up sidepanel layout
	QVBoxLayout *sidePanel = new QVBoxLayout;
	sidePanel->addWidget(mPower);
	sidePanel->addWidget(mMachineInfo);
	sidePanel->addWidget(mMachineParam);

	sidePanel->addWidget(mSimStats);
	sidePanel->addStretch();

	// set up the interactive view
	mScene = new QGraphicsScene(0, 0, 1920, 1080);
	mView = new FS::FactoryView(mScene);
	// Scene building function
	buildDemoScene();

	// Set up the final layout
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(mView);
	layout->addLayout(sidePanel);

	setLayout(layout);

	// set connections
	connect(mView, &FS::FactoryView::activeObject, mMachineInfo, &FS::MachineInformation::activeObject);
	connect(mView, &FS::FactoryView::activeObject, mMachineParam, &FS::MachineParameters::activeObject);
}

void FS::Interface::buildDemoScene()
{
	FS::Import *tmp1 = new FS::Import(300, 250);
	tmp1->setName(QString("Test Machine #1"));
	tmp1->setDescription(QString("This machine is meant to be a test for the pointer of object"));
	tmp1->setSpeed(0.0f);

	FS::Import *tmp2 = new FS::Import(600, 250);
	tmp2->setName(QString("Test Machine #2"));
	tmp2->setDescription(QString("This machine is meant to be a test for the pointer of object"));
	tmp2->setSpeed(0.0f);

	FS::Import *tmp3 = new FS::Import(150, 250);
	tmp3->setName(QString("Test Machine #3"));
	tmp3->setDescription(QString("This machine is meant to be a test for the pointer of object"));
	tmp3->setSpeed(0.0f);

	// add one import machine
	mScene->addItem(tmp1);
	mScene->addItem(tmp2);
	mScene->addItem(tmp3);
	// add one conveyor
	// add one export machine
}


void FS::Interface::tick()
{
	mScene->update();

	mSimStats->setFPS(1000.0 / mElapsedTimer.restart());
}
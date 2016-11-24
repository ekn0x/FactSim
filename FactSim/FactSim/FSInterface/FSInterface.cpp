#include "FSInterface.h"

#include <QPushButton>
#include <QGraphicsScene>

#include <QTimer>

#include <QHBoxLayout>
#include <QVBoxLayout>

// Factory Simulator interface packages
#include "FSFactoryView.h"
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
	mTimer->start(33);
	mElapsedTimer.start();

	// set default power button
	mPower = new QPushButton(QString("Power On"));

	// set default machine informations
	mMachineInfo = new FS::MachineInformation;

	// set default machine parameters

	// set default machine statistics informations

	// Simulation statistics
	mSimStats = new FS::FactSimStats;
	
	// set up sidepanel layout
	QVBoxLayout *sidePanel = new QVBoxLayout;
	sidePanel->addWidget(mPower);
	sidePanel->addWidget(mMachineInfo);

	sidePanel->addWidget(mSimStats);
	sidePanel->addStretch();

	// set up the interactive view
	mScene = new QGraphicsScene(0, 0, 1024, 768);
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
	FS::Import *tmp = new FS::Import;
	tmp->setName(QString("Test Machine #1"));
	tmp->setDescription(QString("This machine is meant to be a test for the pointer of object"));
	tmp->setSpeed(0.0f);

	// add one import machine
	mScene->addItem(tmp);
	// add one conveyor
	// add one export machine
}


void FS::Interface::tick()
{

	mSimStats->setFPS(1000.0 / mElapsedTimer.restart());
}
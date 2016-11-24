#ifndef FACTSIM_H
#define FACTSIM_H

#include <QtWidgets/QMainWindow>
#include "ui_FactSim.h"

class FactSim : public QMainWindow
{
	Q_OBJECT

public:
	FactSim(QWidget *parent = 0);
	~FactSim();

private:
	Ui::FactSimClass ui;
};

#endif // FACTSIM_H

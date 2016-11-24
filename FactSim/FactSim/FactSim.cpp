#include "FactSim.h"

#include "FSInterface\FSInterface.h"

FactSim::FactSim(QWidget *parent)
	: QMainWindow(parent)
{
	setWindowIcon(QIcon(":/FactSim/Icon"));
	ui.setupUi(this);

	setCentralWidget(new FS::Interface);
}

FactSim::~FactSim()
{

}

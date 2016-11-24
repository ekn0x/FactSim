#include "FSMachine.h"

void FS::Machine::setSpeed(qreal s)
{
	if (s > 0) // validate speed
		mSpeed = s;
}

void FS::Machine::setName(QString n)
{
	mName = n;
}

void FS::Machine::setDescription(QString d)
{
	mDescription = d;
}
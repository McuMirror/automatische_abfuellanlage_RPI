/*
 * ####################################################################################
 * Project:		Diplomarbeit: Autmatische Abfüllanlage								  #
 * Host:		Raspberry PI 3B														  #
 * Filename:	mixture.cpp															  #
 *																					  #
 * Developer:	Wögerbauer Stefan													  #
 * E-Mail:		woegste@hotmail.com													  #
 * ####################################################################################
 */

//********************** INCLUDES *****************************************************
#include "mixture.h"
#include "QDebug"

//*************************************************************************************

mixture::mixture(QObject *parent) : QObject(parent)
{

}

void mixture::setAmountContainer_1(int amount)
{
	//set the amount of liquid of container 1
	amountContainer_1 = amount;
}

void mixture::setAmountContainer_2(int amount)
{
	//set the amount of liquid of container 2
	amountContainer_2 = amount;
}

void mixture::setAmountContainer_3(int amount)
{
	//set the amount of liquid of container 3
	amountContainer_3 = amount;
}

void mixture::setAmountContainer_4(int amount)
{
	//set the amount of liquid of container 4
	amountContainer_4 = amount;
}

int mixture::getAmountContainer_1(void)
{
	//return the amount of liquid of container 1
	return amountContainer_1;
}

int mixture::getAmountContainer_2(void)
{
	//return the amount of liquid of container 2
	return amountContainer_2;
}

int mixture::getAmountContainer_3(void)
{
	//return the amount of liquid of container 3
	return amountContainer_3;
}

int mixture::getAmountContainer_4(void)
{
	//return the amount of liquid of container 4
	return amountContainer_4;
}

void mixture::setName(QString name)
{
	//set the name of the mixture
	this->name = name;
}

QString mixture::getName(void)
{
	//return the name of the mixure
	return name;
}

void mixture::getCommandValues(void)
{
	/*
	 * crate a list with the amounts of liquid of each container
	 */

	QList<int> commandValues;

	//set the values
	commandValues.append(amountContainer_1);
	commandValues.append(amountContainer_2);
	commandValues.append(amountContainer_3);
	commandValues.append(amountContainer_4);

	//emit the Signal: send the values
	emit sendCommandValues(commandValues);
}

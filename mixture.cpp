/* ####################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	mixture.cpp
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * Änderungen:
 * Name:	Datum:		Änderung:
 *
 * WS		7.11.2016	Erstellt
 * ####################################################################################
 */

#include "mixture.h"
#include "QDebug"

mixture::mixture(QObject *parent) : QObject(parent)
{

}
void mixture::setAmountContainer_1(int amount)
{
	amountContainer_1 = amount;
}

void mixture::setAmountContainer_2(int amount)
{
	amountContainer_2 = amount;
}

void mixture::setAmountContainer_3(int amount)
{
	amountContainer_3 = amount;
}

void mixture::setAmountContainer_4(int amount)
{
	amountContainer_4 = amount;
}

int mixture::getAmountContainer_1(void)
{
	return amountContainer_1;
}

int mixture::getAmountContainer_2(void)
{
	return amountContainer_2;
}

int mixture::getAmountContainer_3(void)
{
	return amountContainer_3;
}

int mixture::getAmountContainer_4(void)
{
	return amountContainer_4;
}

void mixture::setName(QString name)
{
	this->name = name;
}

QString mixture::getName(void)
{
	return name;
}

void mixture::getCommandValues(void)
{
	QList<int> commandValues;

	commandValues.append(amountContainer_1);
	commandValues.append(amountContainer_2);
	commandValues.append(amountContainer_3);
	commandValues.append(amountContainer_4);

	emit sendCommandValues(commandValues);
}

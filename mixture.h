/* ####################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	mixture.h
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * ####################################################################################
 */

#ifndef MIXTURE_H
#define MIXTURE_H

#include <QObject>

class mixture : public QObject
{
	Q_OBJECT
public:
	explicit mixture(QObject *parent = 0);

signals:
	void sendCommandValues(QList<int> commands);

public slots:
	void setName(QString name);
	QString getName(void);
	void setAmountContainer_1(int amount);
	void setAmountContainer_2(int amount);
	void setAmountContainer_3(int amount);
	void setAmountContainer_4(int amount);
	int  getAmountContainer_1(void);
	int  getAmountContainer_2(void);
	int  getAmountContainer_3(void);
	int  getAmountContainer_4(void);

	void getCommandValues(void);

private:
	QString name;
	int amountContainer_1;
	int amountContainer_2;
	int amountContainer_3;
	int amountContainer_4;
};

#endif // MIXTURE_H

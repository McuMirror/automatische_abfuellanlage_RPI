/* ####################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	mainwindow.h
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * ####################################################################################
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mixture.h"
#include "bluetoothtransmissionserver.h"

#include <QMainWindow>
#include <QTimer>
#include <QByteArray>
#include <QListWidgetItem>
#include <QSettings>
#include <QList>

//Serial Port
#include <QSerialPort>

//Bluetooth
#include <QBluetoothServiceInfo>
#include <QBluetoothSocket>
#include <QBluetoothHostInfo>
#include <QBluetoothLocalDevice>

//************** MACROS ****************************

#define Bluetooth 1

//**************************************************

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void updateTime (void);
	void on_actionMischungen_bearbeiten_triggered();
	void on_action_ber_triggered();
	void on_actionBeenden_triggered();
	void on_actionEinstellungen_triggered();
	void on_pushButton_connect_clicked();
	void on_pushButton_clearSerialTraffic_clicked();
	void saveMixtures(void);
	void readMixtures (void);
	void writeListWidget(void);
	void ButtonSlot(QList<int> valueList);
	void on_comboBox_BT_local_Bluetooth_Adapter_currentIndexChanged(int index);
	void setConatinerAmounts(int Container_1, int Container_2, int Container_3,
							 int Container_4);
	void calculateContainerVolumes(int ConsumtionContainer_1, int ConsumtionContainer_2,
								  int ConsumtionContainer_3, int ConsumtionContainer_4);
	void on_pushButton_fill_C1_clicked();
	void on_pushButton_fill_C2_clicked();
	void on_pushButton_fill_C3_clicked();
	void on_pushButton_fill_C4_clicked();

	//Serial Port
	void sendSerialCommand(QString sender, QString command);
	void readSerialCommand(void);

	//Bluetooth
#if Bluetooth
	void clientConnected(const QString &name);
	void clientDisconnected(const QString &name);
	void BluetoothCommandReceived(QString client, QString command);
	void sendMixes(void);
#endif

private:
	Ui::MainWindow *ui;

	QTimer *clockTimer;     //Updates the clock in status bar
	QTimer *serialTimeOut;

	int screenWidth;
	int screenHeight;

	QSettings settings;

	QList<mixture*> mixtures;

	bool currentlyWorking;
	bool commandReceived;
	bool commandSended;

	//Serial Port
	QSerialPort serial;
	QByteArray response;

	//Bluetooth
	BluetoothTransmissionServer *BluetoothServer;
	QList<QBluetoothHostInfo> localBluetoothAdapters;
};
#endif // MAINWINDOW_H

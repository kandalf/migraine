#include <QSettings>
#include "types.h"
#include "migrainemainwindow.h"
#include "connectiondialog.h"
#include "connectionlistitem.h"

ConnectionDialog::ConnectionDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);
	this->saved = false;
	driversComboBox->addItems(QSqlDatabase::drivers());
	splitter->setStretchFactor(0, 1);
	setupObjectConnections();
	readSettings();
}

void ConnectionDialog::checkConnectionFields()
{
	addConnectionButton->setEnabled(
			(!hostLineEdit->text().isEmpty() && !hostLineEdit->text().isNull()) &&
			(!databaseLineEdit->text().isEmpty() && !databaseLineEdit->text().isNull()) &&
			(!driversComboBox->itemText(driversComboBox->currentIndex()).isEmpty() && !driversComboBox->itemText(driversComboBox->currentIndex()).isNull()) 
		);
}

void ConnectionDialog::addConnection()
{
		ConnectionSettings *settings = new ConnectionSettings();
		
		settings->name = hostLineEdit->text() + "-" + databaseLineEdit->text();
		settings->driver = driversComboBox->itemText(driversComboBox->currentIndex());
		QSqlDatabase db = QSqlDatabase::addDatabase(	settings->driver,	settings->name);
		
		if (settings->driver.contains("ODBC")) {
			QString dbName = "DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1";
			db.setDatabaseName(dbName.arg(databaseLineEdit->text()));
		} else {
			db.setDatabaseName(databaseLineEdit->text());
			db.setHostName(hostLineEdit->text());
			db.setUserName(userLineEdit->text());
			db.setPassword(passwordLineEdit->text());
		}
		connListWidget->addItem(new ConnectionListItem(settings, connListWidget));
		
		//connListWidget->addConnection(settings);
/*		QString connectionName(hostLineEdit->text() + "-" + databaseLineEdit->text());
		QString driverName(driversComboBox->itemText(driversComboBox->currentIndex()));
		
	 	QSqlDatabase db = QSqlDatabase::addDatabase(	driverName,	connectionName);
	
		if (driverName.contains("ODBC")) {
			QString dbName = "DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1";
			db.setDatabaseName(dbName.arg(databaseLineEdit->text()));
		} else {
			db.setDatabaseName(databaseLineEdit->text());
			db.setHostName(hostLineEdit->text());
			db.setUserName(userLineEdit->text());
			db.setPassword(passwordLineEdit->text());
	}
	
	connListWidget->addItem(new QListWidgetItem(hostLineEdit->text() + "-" + databaseLineEdit->text(), connListWidget, QListWidgetItem::Type));*/
}

void ConnectionDialog::readSettings()
{
	QSettings settings(QString(CONFIG_FILE_PATH), QSettings::IniFormat);
	
	settings.beginGroup("Connections");
	QStringList connections = settings.childGroups();
	
	for (int i = 0; i < connections.size(); i++)
	{
		settings.beginGroup(connections.at(i));
		QString driverName = settings.value("driver").toString();
		QSqlDatabase db = QSqlDatabase::addDatabase(driverName, connections.at(i));
		db.setDatabaseName(settings.value("databasename").toString());
		db.setHostName(settings.value("hostname").toString());
		db.setUserName(settings.value("username").toString());
		db.setPassword(settings.value("password").toString());
		connListWidget->addItem(new QListWidgetItem(db.hostName() + "-" + db.databaseName(), connListWidget, QListWidgetItem::Type));
		settings.endGroup();
	}
	
	settings.endGroup();
}

void ConnectionDialog::writeSettings()
{
	QSettings settings(QString(CONFIG_FILE_PATH), QSettings::IniFormat);
	
	settings.beginGroup("Connections");
	
	for (int i = 0; i < QSqlDatabase::connectionNames().size(); i++)
	{
		QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::connectionNames().at(i));
		settings.beginGroup(db.connectionName());
		settings.setValue("databasename", db.databaseName());
		settings.setValue("hostname", db.hostName());
		settings.setValue("username", db.userName());
		settings.setValue("password", db.password());
		settings.setValue("driver", db.driverName());
		settings.endGroup();
	}
	
	settings.endGroup();
	
	settings.sync();
	emit(settingsWritten());
}

void ConnectionDialog::setupObjectConnections()
{
	connect( this, SIGNAL(accepted()), this, SLOT(writeSettings()) );
	connect( connListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(itemConnectionSelected(QListWidgetItem*)) );
}

void ConnectionDialog::itemConnectionSelected(QListWidgetItem *settingsItem)
{
	ConnectionListItem *settings = (ConnectionListItem*)settingsItem;
	
	databaseLineEdit->setText(settings->database());
	hostLineEdit->setText(settings->host());
	userLineEdit->setText(settings->user());
	passwordLineEdit->setText(settings->password());
}
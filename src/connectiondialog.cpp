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
		
		settings->name = hostLineEdit->text() + "-" + databaseLineEdit->text() + " - " + driversComboBox->currentText();
		settings->driver = driversComboBox->currentText();
		
		if (driversComboBox->currentText().contains("ODBC"))
			settings->database = QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(databaseLineEdit->text());
		else
			settings->database = databaseLineEdit->text();
			
		settings->host = hostLineEdit->text();
		settings->user = userLineEdit->text();
		settings->password = passwordLineEdit->text();
		
		addDbConnection(settings);
		connListWidget->addItem(new ConnectionListItem(settings, connListWidget));
}

void ConnectionDialog::readSettings()
{
	QSettings settings(QString(CONFIG_FILE_PATH), QSettings::IniFormat);
	
	settings.beginGroup("Connections");
	QStringList connections = settings.childGroups();
	
	for (int i = 0; i < connections.size(); i++)
	{
		settings.beginGroup(connections.at(i));
	
		ConnectionSettings *connSettings = new ConnectionSettings();
		connSettings->name = connections.at(i);
		connSettings->driver = settings.value("driver").toString();
		connSettings->database =  settings.value("databasename").toString();
		connSettings->host = settings.value("hostname").toString();
		connSettings->port = settings.value("port").toInt();
		connSettings->user = settings.value("username").toString();
		connSettings->password = settings.value("password").toString();
		
		addDbConnection(connSettings);
	
		connListWidget->addItem(new ConnectionListItem(connSettings, connListWidget));
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
	connect( connListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemConnectionSelected(QListWidgetItem*)) );
}

void ConnectionDialog::itemConnectionSelected(QListWidgetItem *settingsItem)
{
	ConnectionListItem *settings = (ConnectionListItem*)settingsItem;
	
	driversComboBox->setCurrentIndex(driversComboBox->findText(settings->driver()));
	databaseLineEdit->setText(settings->database());
	hostLineEdit->setText(settings->host());
	
	if (settings->port())
		portLineEdit->setText(QString::number(settings->port()));
		
	userLineEdit->setText(settings->user());
	passwordLineEdit->setText(settings->password());
	
	deleteConnectionButton->setEnabled(true);
	saveConnectionButton->setEnabled(true);
}

void ConnectionDialog::saveConnection()
{
	ConnectionListItem *item = (ConnectionListItem *)connListWidget->currentItem();
	item->setHost(hostLineEdit->text());
	item->setDatabase(databaseLineEdit->text());
	item->setPort(portLineEdit->text().toInt());
	item->setUser(userLineEdit->text());
	item->setPassword(passwordLineEdit->text());
	item->setDriver(driversComboBox->currentText());
	
	addDbConnection(item->settings());
	writeSettings();
}

void ConnectionDialog::deleteConnection()
{
	
}

void ConnectionDialog::addDbConnection(ConnectionSettings *settings)
{
	QSqlDatabase db = QSqlDatabase::addDatabase(settings->driver, settings->name);
	
	if (QSqlDatabase::contains(settings->name))
		db = QSqlDatabase::database(settings->name);
	else
		db = QSqlDatabase::addDatabase(settings->driver, settings->name);

	db.setDatabaseName(settings->database);
	db.setHostName(settings->host);
	db.setUserName(settings->user);
	db.setPassword(settings->password);
	writeSettings();
}

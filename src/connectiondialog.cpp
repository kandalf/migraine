#include "connectiondialog.h"



ConnectionDialog::ConnectionDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);
	this->saved = false;
	driversComboBox->addItems(QSqlDatabase::drivers());
	splitter->setStretchFactor(0, 1);
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
		QString connectionName(hostLineEdit->text() + "-" + databaseLineEdit->text());
		QString driverName(driversComboBox->itemText(driversComboBox->currentIndex()));
		
	 	connectionHash[connectionName] = QSqlDatabase::addDatabase(
			driverName,
			connectionName
	);
	
	if (driverName.contains("ODBC")) {
		QString dbName = "DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1";
		connectionHash[connectionName].setDatabaseName(dbName.arg(databaseLineEdit->text()));
	} else {
		connectionHash[connectionName].setDatabaseName(databaseLineEdit->text());
		connectionHash[connectionName].setHostName(hostLineEdit->text());
		connectionHash[connectionName].setUserName(userLineEdit->text());
		connectionHash[connectionName].setPassword(passwordLineEdit->text());
	}
	
	connListWidget->addItem(new QListWidgetItem(hostLineEdit->text() + "-" + databaseLineEdit->text(), connListWidget, QListWidgetItem::Type));
}
// place your code here

#include <QSettings>
#include <QFileDialog>
#include <QDir>
//#include <QMessageBox>
#include "types.h"
#include "migrainemainwindow.h"
#include "connectiondialog.h"
#include "connectionlistitem.h"

ConnectionDialog::ConnectionDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
    setupUi(this);
    this->saved = false;
//    driversComboBox->addItems(QSqlDatabase::drivers());
    splitter->setStretchFactor(0, 1);
    setupObjectConnections();
    setupDriversList();
    readSettings();
    saveConnectionButton->setVisible(false);
    this->installEventFilter(this);
}

void ConnectionDialog::checkConnectionFields()
{
    addConnectionButton->setEnabled(
                    (!connectionNameLineEdit->text().isEmpty() && !connectionNameLineEdit->text().isNull()) &&
                    (!hostLineEdit->text().isEmpty() && !hostLineEdit->text().isNull()) &&
                    (!databaseLineEdit->text().isEmpty() && !databaseLineEdit->text().isNull()) &&
                    (!driversComboBox->itemText(driversComboBox->currentIndex()).isEmpty() && !driversComboBox->itemText(driversComboBox->currentIndex()).isNull())
            );
}

void ConnectionDialog::addConnection()
{
    ConnectionSettings *settings = new ConnectionSettings();

//    settings->name = hostLineEdit->text() + "-" + databaseLineEdit->text() + "_" + driversComboBox->itemData(driversComboBox->currentIndex()).toString();
    settings->name = connectionNameLineEdit->text();
//    settings->driver = driversComboBox->currentText();
    settings->driver = driversComboBox->itemData(driversComboBox->currentIndex()).toString();

    if (driversComboBox->currentText().contains("MS Access"))
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
    connect( driversComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(checkSelectedDriver(const QString &)) );
    connect( browseButton, SIGNAL(clicked()), this, SLOT(browseFiles()));
    connect( addConnectionButton, SIGNAL(clicked()), this, SLOT(addConnection()) );
    connect( editConnectionButton, SIGNAL(clicked()), this, SLOT(editConnection()) );
}

void ConnectionDialog::itemConnectionSelected(QListWidgetItem *settingsItem)
{
    ConnectionListItem *settings = (ConnectionListItem*)settingsItem;


    connectionNameLabel->setText(settings->name());
    connectionHostnameLabel->setText(settings->host());
    connectionDriverLabel->setText(driversComboBox->itemText(driversComboBox->findData(settings->driver())));
    connectionUsernameLabel->setText(settings->user());

    if (settings->port())
        connectionPortLabel->setText(QString::number(settings->port()));

    deleteConnectionButton->setEnabled(true);
    editConnectionButton->setEnabled(true);

}

void ConnectionDialog::editConnection()
{
    ConnectionListItem *settings = (ConnectionListItem*)connListWidget->currentItem();

    connectionNameLineEdit->setText(settings->name());
    driversComboBox->setCurrentIndex(driversListIndexes[settings->driver()]);
    databaseLineEdit->setText(settings->database());
    hostLineEdit->setText(settings->host());

    if (settings->port())
            portLineEdit->setText(QString::number(settings->port()));

    userLineEdit->setText(settings->user());
    passwordLineEdit->setText(settings->password());
    saveConnectionButton->setEnabled(true);
    saveConnectionButton->setVisible(true);
    addConnectionButton->setVisible(false);
    connectionsTabWidget->setCurrentIndex(1);
}


void ConnectionDialog::saveConnection()
{
    ConnectionListItem *item = (ConnectionListItem *)connListWidget->currentItem();
    item->setName(connectionNameLineEdit->text());
    item->setHost(hostLineEdit->text());
    item->setDatabase(databaseLineEdit->text());
    item->setPort(portLineEdit->text().toInt());
    item->setUser(userLineEdit->text());
    item->setPassword(passwordLineEdit->text());
    item->setDriver(driversListIndexes.key(driversComboBox->currentIndex()));

    addDbConnection(item->settings());
    connListWidget->currentItem()->setText(connectionNameLineEdit->text());
    writeSettings();
    restoreUIState();

}

void ConnectionDialog::deleteConnection()
{
        QSettings settings(CONFIG_FILE_PATH, QSettings::IniFormat, this);

	ConnectionListItem *item = (ConnectionListItem*)connListWidget->currentItem();
	
	QSqlDatabase::removeDatabase(item->name());
	connListWidget->takeItem(connListWidget->currentRow());

        settings.remove(QString("Connections/%1").arg(item->name()));

        settings.sync();
	emit(settingsWritten());
}

void ConnectionDialog::addDbConnection(ConnectionSettings *settings)
{
        QSqlDatabase db;// = QSqlDatabase::addDatabase(settings->driver, settings->name);
//        QMessageBox::information(this, "DRIVER", settings->driver);
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

void ConnectionDialog::setupDriversList()
{
    bool odbcSupport = false;
    int index = 0;

    foreach(QString driverName, QSqlDatabase::drivers())
    {
        QString driverLabel;
        if (driverName == "QMYSQL")
            driverLabel = "MySQL >= 4";
        else if(driverName == "QMYSQL3")
            driverLabel = "MySQL <= 3";
        else if(driverName == "QPSQL")
            driverLabel = "PostgreSQL 8.x";
        else if(driverName == "QPSQL7")
            driverLabel = "PostgreSQL 7.x";
        else if(driverName == "QSQLITE")
            driverLabel = "SQLite";
        else if(driverName == "QODBC")
        {
            driverLabel = "ODBC";
            odbcSupport = true;
        }
        else if(driverName == "QODBC3")
            driverLabel = "ODBC 3";
        else
            break;
        driversListIndexes[driverName] = index++;
        driversComboBox->addItem(driverLabel, driverName);
    }

    if (odbcSupport)
        driversComboBox->addItem("MS Access 97/2000 File", "QODBC");
}

void ConnectionDialog::checkSelectedDriver(const QString &driverName)
{
    if (driverName.contains("MS Access") || driverName.contains("SQLite"))
        browseButton->show();
    else
        browseButton->hide();
}

void ConnectionDialog::browseFiles()
{
    QString filter;
    if (driversComboBox->currentText().contains("MS Access"))
        filter = "*.mdb";
    else if (driversComboBox->currentText().contains("SQLite"))
        filter = "*.*";
    else
        filter = "*";

    databaseLineEdit->setText(QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), filter));
}

bool ConnectionDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this && (event->type() == QEvent::Close || event->type() == QEvent::Hide))
        restoreUIState();

    return false;
}


void ConnectionDialog::restoreUIState()
{
    saveConnectionButton->setEnabled(false);
    saveConnectionButton->setVisible(false);
    editConnectionButton->setEnabled(false);
    addConnectionButton->setEnabled(false);
    addConnectionButton->setVisible(true);
    connectionsTabWidget->setCurrentIndex(0);
    clearFields();
}

void ConnectionDialog::clearFields()
{
    connectionNameLineEdit->clear();
    driversComboBox->setCurrentIndex(0);
    databaseLineEdit->clear();
    hostLineEdit->clear();
    portLineEdit->clear();
    userLineEdit->clear();
    passwordLineEdit->clear();
}

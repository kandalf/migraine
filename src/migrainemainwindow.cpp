#include <QSqlDatabase>
#include <QStringListModel>
#include <QSqlError>
#include <QSettings>

#include "migrainemainwindow.h"
#include "connectiondialog.h"

MigraineMainWindow::MigraineMainWindow( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	QCoreApplication::setApplicationName("Migraine");
	QCoreApplication::setOrganizationDomain("snichaos.com");
	QCoreApplication::setOrganizationName("SNI");
	
	setupUi(this);
	setupObjectConnections();
	refreshConnections();
	hSplitter->setStretchFactor(0,1);
	vSplitter->setStretchFactor(1,1);
	connDialog = 0;
	
	_settings = new QSettings("conf/settings.ini", QSettings::IniFormat, this);
	readSettings();
}

MigraineMainWindow::~MigraineMainWindow()
{
	for (int i = 0; i < QSqlDatabase::connectionNames().size(); i++)
	{
		if (QSqlDatabase::database(QSqlDatabase::connectionNames().at(i)).isOpen())
			QSqlDatabase::database(QSqlDatabase::connectionNames().at(i)).close();
	}
}

void MigraineMainWindow::setupObjectConnections()
{
	connect( dbSrcConnCombo, SIGNAL(activated(const QString &)), this, SLOT(connectionSelected(const QString&)) );
}

void MigraineMainWindow::showConnectionDialog()
{
	if (!connDialog) {
		connDialog = new ConnectionDialog(this);
		connect( connDialog, SIGNAL(accepted()), this, SLOT(refreshConnections()) );
		connect( connDialog, SIGNAL(settingsWritten()), this, SLOT(readSettings()) );
	}
		
	connDialog->show();
}

void MigraineMainWindow::refreshConnections()
{
	for (int i = 0; i < dbSrcConnCombo->count(); i++)
		dbSrcConnCombo->removeItem(i);
	dbSrcConnCombo->addItem("Select a connection");
	dbSrcConnCombo->addItems(QSqlDatabase::connectionNames());
	
	if (dbSrcConnCombo->count() > 1)
		dbSrcConnCombo->showPopup();
}

void MigraineMainWindow::connectionSelected(const QString &name)
{
	QSqlDatabase db = QSqlDatabase::database(name, true);
	if (!db.isOpen())
	{
		logTextEdit->append("Cannot Open Database: " + db.lastError().text());
		return;
	}
		
	QStringListModel *model = new QStringListModel(db.tables(QSql::Tables), connTablesView);
	connTablesView->setModel(model);
}

void MigraineMainWindow::readSettings()
{
	// do some read task here
}

void MigraineMainWindow::writeSettings()
{
	// do some write task here
}

QSettings* MigraineMainWindow::settings()
{
	return _settings;
}
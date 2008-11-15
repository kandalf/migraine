#include <QStringListModel>
#include <QSqlError>
#include <QSqlDriver>
#include <QSettings>

#include "migrainemainwindow.h"
#include "connectiondialog.h"
#include "tableinfo.h"
#include "tableinfomodel.h"

MigraineMainWindow::MigraineMainWindow( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	QCoreApplication::setApplicationName("Migraine");
	QCoreApplication::setOrganizationDomain("snichaos.com");
	QCoreApplication::setOrganizationName("SNI");
	
	setupUi(this);
	
//	hSplitter->setStretchFactor(0,1);
	//vSplitter->setStretchFactor(1,1);
	connDialog = new ConnectionDialog(this);
	
	_settings = new QSettings("conf/settings.ini", QSettings::IniFormat, this);
	readSettings();
	setupObjectConnections();
	refreshConnections();
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
	connect( dbSrcConnCombo, SIGNAL(activated(const QString &)), this, SLOT(srcConnectionSelected(const QString&)) );
	connect( dbTargetConnCombo, SIGNAL(activated(const QString &)), this, SLOT(tgtConnectionSelected(const QString&)) );
	connect( actionConnections, SIGNAL(activated()), connDialog, SLOT(show()) );
	connect( connDialog, SIGNAL(accepted()), this, SLOT(refreshConnections()) );
	connect( connDialog, SIGNAL(settingsWritten()), this, SLOT(readSettings()) );
	
}

void MigraineMainWindow::refreshConnections()
{
	for (int i = 0; i < dbSrcConnCombo->count(); i++)
	{
		dbSrcConnCombo->removeItem(i);
		dbTargetConnCombo->removeItem(i);
	}
	QString emptyOption(tr("Select a connection"));
	dbSrcConnCombo->addItem(emptyOption);
	dbTargetConnCombo->addItem(emptyOption);
	
	dbSrcConnCombo->addItems(QSqlDatabase::connectionNames());
	dbTargetConnCombo->addItems(QSqlDatabase::connectionNames());
	
	if (dbSrcConnCombo->count() > 1)
		dbSrcConnCombo->showPopup();
		
	if (dbTargetConnCombo->count() > 1)
		dbTargetConnCombo->showPopup();
}

void MigraineMainWindow::srcConnectionSelected(const QString &name)
{
	QSqlDatabase db = QSqlDatabase::database(name, true);
	if (!db.isOpen())
	{
		logTextEdit->append("Cannot Open Database: " + db.lastError().text());
		return;
	}
		
	srcDbTreeView->setModel(buildTreeModel(db));
}

void MigraineMainWindow::tgtConnectionSelected(const QString &name)
{
	QSqlDatabase db = QSqlDatabase::database(name, true);
	if (!db.isOpen())
	{
		logTextEdit->append("Cannot Open Database: " + db.lastError().text());
		return;
	}
		
	targetDbTreeView->setModel(buildTreeModel(db));
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

TableInfoModel* MigraineMainWindow::buildTreeModel(QSqlDatabase db)
{
	QList<TableInfo*> data;
	QSqlDriver *driver = db.driver();
	QStringList tables = db.tables(QSql::Tables);
	
	for(int i = 0; i < tables.count(); i++)
	{
		data << new TableInfo(tables.at(i), driver->record(tables.at(i)));
	}
	
	//TableInfoModel *model = new TableInfoModel(data);
	//srcDbTreeView->setModel(model);
	return new TableInfoModel(data);
}
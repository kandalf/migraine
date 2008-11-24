#include <QStringListModel>
#include <QSqlError>
#include <QSqlDriver>
#include <QSettings>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QSqlField>
#include "migrainemainwindow.h"
#include "connectiondialog.h"
#include "tableinfo.h"
#include "tableinfomodel.h"
#include "dbanalyst.h"
#include "migrationtablematch.h"
#include "maptablenamematchmodel.h"


MigraineMainWindow::MigraineMainWindow( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	QCoreApplication::setApplicationName("Migraine");
    QCoreApplication::setOrganizationDomain("migraine.com");
    QCoreApplication::setOrganizationName("MIGRAINE");
	
	setupUi(this);
	
	connDialog = new ConnectionDialog(this);
    _settings = new QSettings("conf/settings.ini", QSettings::IniFormat, this);
    analyst = new DBAnalyst(this);
    analyst->setCreateTables(true);
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
    connect( analyst, SIGNAL(exactMatchFound(const QString&)), this, SLOT(exactMatch(const QString&)) );
    connect( analyst, SIGNAL(nameMatchFound(const QString&)), this, SLOT(matchByName(const QString&)) );
    connect( analyst, SIGNAL(noMatchFound(const QString&)), this, SLOT(noMatch(const QString&)) );

    connect( nameMatchListView, SIGNAL(pressed(const QModelIndex &)), this, SLOT(nameMatchSelected(const QModelIndex &)) );

    connect( tgtColumnsTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(tgtColumnSelected()) );
    connect( addMapColumnButton, SIGNAL(clicked()), this, SLOT(addMapColumn()) );

    connect( createTablesCheckbox, SIGNAL(toggled(bool)), analyst, SLOT(setCreateTables(bool)) );
    connect( previewMigrationButton, SIGNAL(clicked()), this, SLOT(previewMigration()) );
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
        QMessageBox::critical(this, tr("Error"), tr("Cannot Open Database: %1").arg(db.lastError().text()));
        logTextEdit->append(tr("Cannot Open Database: %1").arg(db.lastError().text()));
		return;
	}
		
	srcDbTreeView->setModel(buildTreeModel(db));
}

void MigraineMainWindow::tgtConnectionSelected(const QString &name)
{
	QSqlDatabase db = QSqlDatabase::database(name, true);
	if (!db.isOpen())
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot Open Database: %1").arg(db.lastError().text()));
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
	
    return new TableInfoModel(data);
}

void MigraineMainWindow::analyzeDatabases()
{

    stepsTabWidget->setCurrentIndex(1);
    analyst->analyzeDatabases(
            static_cast<TableInfoModel*>(srcDbTreeView->model())->toTableInfo(),
            static_cast<TableInfoModel*>(targetDbTreeView->model())->toTableInfo()
    );
}

void MigraineMainWindow::exactMatch(const QString &name)
{
    QStringList names;
    QStringListModel *model;

    if (exactMatchListView->model())
    {
        model = static_cast<QStringListModel*>(exactMatchListView->model());
        names = model->stringList();
    }
    else
    {
        model = new QStringListModel(exactMatchListView);
    }

    names << name;
    model->setStringList(names);

    exactMatchListView->setModel(model);
}

void MigraineMainWindow::matchByName(const QString &name)
{
    QStringList names;
    QStringListModel *model;

    if (nameMatchListView->model())
    {
        model = static_cast<QStringListModel*>(nameMatchListView->model());
        names = model->stringList();
    }
    else
    {
        model = new QStringListModel(nameMatchListView);
    }

    names << name;
    model->setStringList(names);

    nameMatchListView->setModel(model);
}

void MigraineMainWindow::noMatch(const QString &name)
{
    QStringList names;
    QStringListModel *model;
    if (noMatchListView->model())
    {
        model = static_cast<QStringListModel*>(noMatchListView->model());
        names = model->stringList();
    }
    else
    {
        model = new QStringListModel(noMatchListView);
    }

    names << name;
    model->setStringList(names);

    noMatchListView->setModel(model);
}

void MigraineMainWindow::nameMatchSelected(const QModelIndex &index)
{
    MigrationTableMatch *tableMatch = analyst->getNameMatchTable(index.data(Qt::DisplayRole).toString());

    if(!tableMatch)
    {
        QMessageBox::critical(this, tr("Error"), tr("Invalid tables"), QMessageBox::Ok);
        return;
    }

    buildColumnsItems(tableMatch->source(), SOURCE_COLUMNS);
    buildColumnsItems(tableMatch->target(), TARGET_COLUMNS);
    enableColumnsWidgets();
    addMapColumnButton->setEnabled(false);
    delMapColumnButton->setEnabled(false);
}


void MigraineMainWindow::buildColumnsItems(TableInfo *info, int type)
{
    QTableWidget *tableWidget;

    if (type == SOURCE_COLUMNS)
        tableWidget = srcColumnsTableWidget;
    else if(type == TARGET_COLUMNS)
        tableWidget = tgtColumnsTableWidget;

    for (int i = 0; i < tableWidget->rowCount(); i++)
        tableWidget->removeRow(i);

    for (int i = 0; i < info->fieldNames().count(); i++)
    {
        QTableWidgetItem *fieldName = new QTableWidgetItem(info->fieldName(i), QTableWidgetItem::Type);
        QTableWidgetItem *fieldType = new QTableWidgetItem(info->fieldType(i), QTableWidgetItem::Type);
        tableWidget->insertRow(i);
        tableWidget->setItem(i, 0, fieldName);
        tableWidget->setItem(i, 1, fieldType);
    }
}

void MigraineMainWindow::enableColumnsWidgets()
{
    srcColumnsFrame->setEnabled(true);
    tgtColumnsFrame->setEnabled(true);
    mapColumnsFrame->setEnabled(true);
}

void MigraineMainWindow::tgtColumnSelected()
{
    if (srcColumnsTableWidget->currentItem())
        addMapColumnButton->setEnabled(true);
}

void MigraineMainWindow::addMapColumn()
{
    analyst->setTableMatch(
                nameMatchListView->currentIndex().data(Qt::DisplayRole).toString(),
                srcColumnsTableWidget->item(srcColumnsTableWidget->currentRow(), 0)->data(Qt::DisplayRole).toString(),
                srcColumnsTableWidget->item(tgtColumnsTableWidget->currentRow(), 0)->data(Qt::DisplayRole).toString()
                );
    refreshMapView(nameMatchListView->currentIndex().data(Qt::DisplayRole).toString());
    srcColumnsTableWidget->removeRow(srcColumnsTableWidget->currentRow());
    tgtColumnsTableWidget->removeRow(tgtColumnsTableWidget->currentRow());
    addMapColumnButton->setEnabled(false);
}

void MigraineMainWindow::refreshMapView(const QString &tableName)
{
    MapTableNameMatchModel *model;
    MigrationTableMatch *table = analyst->getNameMatchTable(tableName);

    if (mapColumnsTreeView->model())
    {
        model = static_cast<MapTableNameMatchModel*>(mapColumnsTreeView->model());
        model->addOrCreateTableMatch(table);
    }
    else
    {
        QHash<QString, MigrationTableMatch*> data;
        data[tableName] = table;
        model = new MapTableNameMatchModel(data, mapColumnsTreeView);
        mapColumnsTreeView->setModel(model);
    }
}

void MigraineMainWindow::previewMigration()
{
    QStringListModel *copiedModel = new QStringListModel(analyst->exactMatches(), copiedTablesListView);
    QStringListModel *migratedModel = new QStringListModel(analyst->nameMatches(), migratedTablesListView);
    QStringListModel *createdModel = new QStringListModel(analyst->noMatches(), createdTablesListView);

    copiedTablesListView->setModel(copiedModel);
    migratedTablesListView->setModel(migratedModel);
    createdTablesListView->setModel(createdModel);
    stepsTabWidget->setCurrentIndex(2);
}

void MigraineMainWindow::resetMigration()
{

}

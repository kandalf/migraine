#include <QStringListModel>
#include <QSqlError>
#include <QSqlDriver>
#include <QSettings>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QSqlField>
#include "migrainemainwindow.h"
#include "connectiondialog.h"
//#include "settingsdialog.h"
#include "tableinfo.h"
#include "tableinfomodel.h"
#include "dbanalyst.h"
#include "dbmigrator.h"
#include "migrationtablematch.h"
#include "maptablenamematchmodel.h"
#include "migrationprogresswidget.h"
#include "postgisdialog.h"

MigraineMainWindow::MigraineMainWindow( QWidget * parent, Qt::WFlags f )
	: QMainWindow(parent, f)
{
    QCoreApplication::setApplicationName("Migraine");
    QCoreApplication::setOrganizationDomain("migraine.com");
    QCoreApplication::setOrganizationName("MIGRAINE");
	
    setupUi(this);
	
    connDialog = new ConnectionDialog(this);
    pgDialog = new PostGISDialog(this);

    _settings = new QSettings(CONFIG_FILE_PATH, QSettings::IniFormat, this);

    analyst = new DBAnalyst(this);
    analyst->setCreateTables(true);
    migrator = new DBMigrator(this->analyst, this);
    progressWidget = new MigrationProgressWidget(this);
    readSettings();
    setupObjectConnections();
    refreshConnections();

    previewMigrationButton->addAction(actionPre_view_Migration);
    resetMigrationButton->addAction(actionReset_Migration);
    migrateButton->addAction(actionMigrate);
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
    connect( dbTgtConnCombo, SIGNAL(activated(const QString &)), this, SLOT(tgtConnectionSelected(const QString&)) );
    connect( actionConnections, SIGNAL(activated()), connDialog, SLOT(show()) );

    connect( connDialog, SIGNAL(accepted()), this, SLOT(refreshConnections()) );
    connect( connDialog, SIGNAL(settingsWritten()), this, SLOT(readSettings()) );

    connect( pgDialog, SIGNAL(migrateGeometries(bool)), migrator, SLOT(setMigrateGeometries(bool)) );
    connect( pgDialog, SIGNAL(migrateAsText(bool)), migrator, SLOT(setMigrateGeometriesAsText(bool)) );
    connect( pgDialog, SIGNAL(ignoreGeometries(bool)), migrator, SLOT(setIgnoreGeometries(bool)) );
    connect( pgDialog, SIGNAL(accepted()), progressWidget, SLOT(show()) );
    connect( pgDialog, SIGNAL(accepted()), migrator, SLOT(start()) );

    connect( analyst, SIGNAL(exactMatchFound(const QString&)), this, SLOT(exactMatch(const QString&)) );
    connect( analyst, SIGNAL(nameMatchFound(const QString&)), this, SLOT(matchByName(const QString&)) );
    connect( analyst, SIGNAL(noMatchFound(const QString&)), this, SLOT(noMatch(const QString&)) );
    connect( analyst, SIGNAL(setMatchError(const QString&)), logTextEdit, SLOT(append(const QString&)));
    connect( analyst, SIGNAL(analysisDone(bool)), actionPre_view_Migration, SLOT(setEnabled(bool)) );
    connect( analyst, SIGNAL(postGISFoundOnSource(bool)), migrator, SLOT(setPostGISOnSource(bool)) );
    connect( analyst, SIGNAL(postGISFoundOnTarget(bool)), migrator, SLOT(setPostGISOnTarget(bool)) );

    connect( actionMigrate, SIGNAL(activated()), this, SLOT(startMigration()) );

    connect( nameMatchListView, SIGNAL(pressed(const QModelIndex &)), this, SLOT(nameMatchSelected(const QModelIndex &)) );
    connect( tgtColumnsTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(tgtColumnSelected()) );
    connect( addMapColumnButton, SIGNAL(clicked()), this, SLOT(addMapColumn()) );

    connect( createTablesCheckbox, SIGNAL(toggled(bool)), analyst, SLOT(setCreateTables(bool)) );

    connect( migrator, SIGNAL(migrationError(const QString &)), this, SLOT(showErrorMessage(const QString&)));
    connect( migrator, SIGNAL(insertProgress(const int&, const int&)), progressWidget, SLOT(setInsertProgress(const int&, const int &)) );
    connect( migrator, SIGNAL(tablesToCopy(const int&)), progressWidget, SLOT(setCopyProgressTotal(const int&)) );
    connect( migrator, SIGNAL(tablesToMigrate(const int&)), progressWidget, SLOT(setMigrateProgressTotal(const int&)) );
    connect( migrator, SIGNAL(tablesToCreate(const int&)), progressWidget, SLOT(setCreateProgressTotal(const int&)) );
    connect( migrator, SIGNAL(migrationDone()), progressWidget, SLOT(close()) );
    connect( migrator, SIGNAL(migrationDone(const int&, const int&, const int&)), this, SLOT(showMigrationStats(const int&, const int&, const int&)) );
    connect( migrator, SIGNAL(tableCopyStarted(const QString&, const int&)), this, SLOT(updateCopyTablesProgress(const QString&, const int&)) );
    connect( migrator, SIGNAL(tableMigrationStarted(const QString&, const int&)), this, SLOT(updateMigrateTablesProgress(const QString&, const int&)) );
    connect( migrator, SIGNAL(tableCreationStarted(const QString&, const int&)), this, SLOT(updateCreateTablesProgress(const QString&, const int&)) );

    connect( actionOutput, SIGNAL(changed()), this, SLOT(writeSettings()) );
}

void MigraineMainWindow::refreshConnections()
{
    for (int i = 0; i < dbSrcConnCombo->count(); i++)
    {
        dbSrcConnCombo->removeItem(i);
        dbTgtConnCombo->removeItem(i);
    }
    QString emptyOption(tr("Select a connection"));
    dbSrcConnCombo->addItem(emptyOption);
    dbTgtConnCombo->addItem(emptyOption);

    dbSrcConnCombo->addItems(QSqlDatabase::connectionNames());
    dbTgtConnCombo->addItems(QSqlDatabase::connectionNames());

    if (dbSrcConnCombo->count() > 1)
            dbSrcConnCombo->showPopup();

    if (dbTgtConnCombo->count() > 1)
            dbTgtConnCombo->showPopup();
}

void MigraineMainWindow::srcConnectionSelected(const QString &name)
{
    if (name.contains(tr("Select a connection")))
    {
        srcDbTreeView->setModel(new QStringListModel());
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(name, true);
    if (!db.isOpen())
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot Open Database: %1").arg(db.lastError().text()));
        logTextEdit->append(tr("Cannot Open Database: %1").arg(db.lastError().text()));
        action_Analyze->setEnabled(false);
        return;
    }
		
    srcDbTreeView->setModel(buildTreeModel(db));
    this->migrator->setSourceConnectionName(name);
    action_Analyze->setEnabled(
                        !migrator->sourceConnectionName().isEmpty() && !migrator->sourceConnectionName().isNull()
                        && !migrator->targetConnectionName().isEmpty() && !migrator->targetConnectionName().isNull()
                        );
}

void MigraineMainWindow::tgtConnectionSelected(const QString &name)
{
    if (name.contains(tr("Select a connection")))
    {
        tgtDbTreeView->setModel(new QStringListModel());
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(name, true);
    if (!db.isOpen())
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot Open Database: %1").arg(db.lastError().text()));
		logTextEdit->append("Cannot Open Database: " + db.lastError().text());
                action_Analyze->setEnabled(false);
		return;
    }
		
    tgtDbTreeView->setModel(buildTreeModel(db));
    this->migrator->setTargetConnectionName(name);
    action_Analyze->setEnabled(
                        !migrator->sourceConnectionName().isEmpty() && !migrator->sourceConnectionName().isNull()
                        && !migrator->targetConnectionName().isEmpty() && !migrator->targetConnectionName().isNull()
                        );
}

void MigraineMainWindow::readSettings()
{
    // do some read task here
    _settings->beginGroup("MainWindow");
    actionOutput->setChecked(_settings->value("show_output").toBool());
    _settings->endGroup();
}

void MigraineMainWindow::writeSettings()
{
    _settings->beginGroup("MainWindow");
    _settings->setValue("show_output", actionOutput->isChecked());
    _settings->endGroup();
    _settings->sync();
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
    resetTableLists();
    analyst->analyzeDatabases(
            static_cast<TableInfoModel*>(srcDbTreeView->model())->toTableInfo(),
            static_cast<TableInfoModel*>(tgtDbTreeView->model())->toTableInfo()
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

    while (tableWidget->rowCount() > 0)
        tableWidget->removeRow(0);

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
                tgtColumnsTableWidget->item(tgtColumnsTableWidget->currentRow(), 0)->data(Qt::DisplayRole).toString()
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
    QStringListModel *migratedModel = new QStringListModel(analyst->tablesToMigrate(), migratedTablesListView);
    QStringListModel *createdModel = new QStringListModel(analyst->noMatches(), createdTablesListView);

    copiedTablesListView->setModel(copiedModel);
    migratedTablesListView->setModel(migratedModel);
    createdTablesListView->setModel(createdModel);
    stepsTabWidget->setCurrentIndex(2);
    actionMigrate->setEnabled(true);
    actionReset_Migration->setEnabled(true);

}

void MigraineMainWindow::resetMigration()
{
    analyst->reset();
    resetTableLists();
    stepsTabWidget->setCurrentIndex(0);
    srcDbTreeView->setModel(new QStringListModel());
    tgtDbTreeView->setModel(new QStringListModel());
    action_Analyze->setEnabled(false);
    actionPre_view_Migration->setEnabled(false);
    actionMigrate->setEnabled(false);
    actionReset_Migration->setEnabled(false);
    dbSrcConnCombo->setCurrentIndex(dbSrcConnCombo->findText(tr("Select a connection")));
    dbTgtConnCombo->setCurrentIndex(dbTgtConnCombo->findText(tr("Select a connection")));
}

void MigraineMainWindow::startMigration()
{
//    if (migrator->postGISOnSource() && migrator->postGISOnTarget())
//    {
//        pgDialog->exec() ;
//    }
//    else
//    {
        progressWidget->show();
        migrator->start();
//    }

}

void MigraineMainWindow::showErrorMessage(const QString &message)
{
    logTextEdit->append(message);
}

void MigraineMainWindow::showMigrationStats(const int &copied, const int &migrated, const int &created)
{
    QString msg(tr("Migration Finished\nCopied: %1\nMigrated: %2\nCreated: %3").arg(copied).arg(migrated).arg(created));

    QMessageBox::information(this, tr("Finish"), msg);
    logTextEdit->append(msg);
}

void MigraineMainWindow::updateCopyTablesProgress(const QString &tableName, const int &index)
{
    progressWidget->setWindowTitle(tr("Copying... %1").arg(tableName));
    progressWidget->setCopyProgress(index);
}

void MigraineMainWindow::updateMigrateTablesProgress(const QString &tableName, const int &index)
{
    progressWidget->setWindowTitle(tr("Migrating... %1").arg(tableName));
    progressWidget->setMigrateProgress(index);
}

void MigraineMainWindow::updateCreateTablesProgress(const QString &tableName, const int &index)
{
    progressWidget->setWindowTitle(tr("Creating... %1").arg(tableName));
    progressWidget->setCreateProgress(index);
}

void MigraineMainWindow::resetTableLists()
{
    exactMatchListView->setModel(new QStringListModel());
    nameMatchListView->setModel(new QStringListModel());
    noMatchListView->setModel(new QStringListModel());
    copiedTablesListView->setModel(new QStringListModel());
    migratedTablesListView->setModel(new QStringListModel());
    createdTablesListView->setModel(new QStringListModel());
}

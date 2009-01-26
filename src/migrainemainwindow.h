#ifndef MIGRAINEMAINWINDOW_H
#define MIGRAINEMAINWINDOW_H


#ifndef SOURCE_COLUMNS
#define SOURCE_COLUMNS 1
#endif

#ifndef TARGET_COLUMNS
#define TARGET_COLUMNS 2
#endif

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDir>
#include "ui_migrainewindow.h"

#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH  "conf/settings.ini"
#endif


class ConnectionDialog;
//class SettingsDialog;
class MigrationProgressWidget;
class QSettings;
class QTableWidgetItem;
class TableInfo;
class TableInfoModel;
class DBAnalyst;
class DBMigrator;

class MigraineMainWindow : public QMainWindow, public Ui::MigraineWindow
{
Q_OBJECT

	public:
            MigraineMainWindow( QWidget * parent = 0, Qt::WFlags f = 0 );
            ~MigraineMainWindow();
            QSettings* settings();
		
	public slots:
            void refreshConnections();
            void readSettings();
            void writeSettings();
            void analyzeDatabases();

	private slots:
            void srcConnectionSelected(const QString&);
            void tgtConnectionSelected(const QString&);
            void exactMatch(const QString&);
            void matchByName(const QString&);
            void noMatch(const QString&);
            void nameMatchSelected(const QModelIndex &);
            void buildColumnsItems(TableInfo*, int);
            void enableColumnsWidgets();
            void tgtColumnSelected();
            void addMapColumn();
            void refreshMapView(const QString &tableName);
            void previewMigration();
            void resetMigration();
            void startMigration();
            void showErrorMessage(const QString &error);
            void showMigrationStats(const int &copied, const int &migrated, const int &created);
            void updateCopyTablesProgress(const QString &tableName, const int &index);
            void updateMigrateTablesProgress(const QString &tableName, const int &index);
            void updateCreateTablesProgress(const QString &tableName, const int &index);
            void resetTableLists();

	private:
            void setupObjectConnections();

            TableInfoModel* buildTreeModel(QSqlDatabase db);
            ConnectionDialog *connDialog;
    //        SettingsDialog *connDialog;
            MigrationProgressWidget *progressWidget;
            DBAnalyst *analyst;
            DBMigrator *migrator;
            QSettings *_settings;
};
#endif





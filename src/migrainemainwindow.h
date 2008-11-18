#ifndef MIGRAINEMAINWINDOW_H
#define MIGRAINEMAINWINDOW_H

#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH "/home/leonardo/development/GISWorking/migraine/conf/settings.ini"
#endif

#ifndef SOURCE_COLUMNS
#define SOURCE_COLUMNS 1
#endif

#ifndef TARGET_COLUMNS
#define TARGET_COLUMNS 2
#endif

#include <QMainWindow>
#include <QSqlDatabase>
#include "ui_migrainewindow.h"



class ConnectionDialog;
class QSettings;
class QTableWidgetItem;
class TableInfo;
class TableInfoModel;
class DBAnalyst;

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
        void matchByName(const QString&);
        void noMatch(const QString&);
        void nameMatchSelected(const QModelIndex &);
        void buildColumnsItems(TableInfo*, int);
        void enableColumnsWidgets();
        void tgtColumnSelected();
        void addMapColumn();

	private:
		void setupObjectConnections();
		TableInfoModel* buildTreeModel(QSqlDatabase db);
		
        ConnectionDialog *connDialog;
        DBAnalyst *analyst;
		QSettings *_settings;
};
#endif





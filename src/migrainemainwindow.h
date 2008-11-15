#ifndef MIGRAINEMAINWINDOW_H
#define MIGRAINEMAINWINDOW_H

#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH "conf/settings.ini"
#endif

#include <QMainWindow>
#include <QSqlDatabase>
#include "ui_migrainewindow.h"



class ConnectionDialog;
class QSettings;
class TableInfoModel;

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
		
	private slots:
		void srcConnectionSelected(const QString&);
		void tgtConnectionSelected(const QString&);
		
	private:
		void setupObjectConnections();
		TableInfoModel* buildTreeModel(QSqlDatabase db);
		
		ConnectionDialog *connDialog;
		QSettings *_settings;
};
#endif





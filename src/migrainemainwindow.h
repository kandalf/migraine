#ifndef MIGRAINEMAINWINDOW_H
#define MIGRAINEMAINWINDOW_H

#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH "conf/settings.ini"
#endif

#include <QMainWindow>
#include "ui_migrainewindow.h"



class ConnectionDialog;
class QSettings;

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
		void connectionSelected(const QString&);
		
	private:
		void setupObjectConnections();
	
		ConnectionDialog *connDialog;
		QSettings *_settings;
};
#endif





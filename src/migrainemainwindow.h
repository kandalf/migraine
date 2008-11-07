#ifndef MIGRAINEMAINWINDOW_H
#define MIGRAINEMAINWINDOW_H
//
#include <QMainWindow>
#include "ui_migrainewindow.h"
//
class MigraineMainWindow : public QMainWindow, public Ui::MigraineWindow
{
Q_OBJECT
	public:
		MigraineMainWindow( QWidget * parent = 0, Qt::WFlags f = 0 );
	
	private slots:
		void showConnectionDialog();
};
#endif





#include <QApplication>
#include "migrainemainwindow.h"
//
int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	MigraineMainWindow win;
	win.showMaximized(); 
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}

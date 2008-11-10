TEMPLATE = app
QT = gui core sql xml
CONFIG += qt \
 debug \
 warn_on \
 console \
 windows
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/migrainewindow.ui ui/connectiondialog.ui
HEADERS = src/migrainemainwindow.h src/connectiondialog.h src/types.h src/connectionlistitem.h
SOURCES = src/migrainemainwindow.cpp src/main.cpp src/connectiondialog.cpp src/connectionlistitem.cpp

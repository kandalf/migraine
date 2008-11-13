TEMPLATE = app
QT = core sql xml gui
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
HEADERS = src/migrainemainwindow.h \
 src/connectiondialog.h \
 src/types.h \
 src/connectionlistitem.h \
 src/treeitem.h \
 src/treemodel.h
SOURCES = src/migrainemainwindow.cpp \
 src/main.cpp \
 src/connectiondialog.cpp \
 src/connectionlistitem.cpp \
 src/treeitem.cpp \
 src/treemodel.cpp

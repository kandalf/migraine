TEMPLATE = app
QT = core \
    sql \
    xml \
    gui
CONFIG += qt \
    debug \
    warn_on \
    console \
    windows
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/migrainewindow.ui \
    ui/connectiondialog.ui
HEADERS = src/connectiondialog.h \
    src/types.h \
    src/connectionlistitem.h \
    src/treeitem.h \
    src/treemodel.h \
    src/tableinfo.h \
    src/tableinfomodel.h \
    src/dbanalyst.h \
    src/migrationtablematch.h \
    src/migrainemainwindow.h
SOURCES = src/migrainemainwindow.cpp \
    src/main.cpp \
    src/connectiondialog.cpp \
    src/connectionlistitem.cpp \
    src/treeitem.cpp \
    src/treemodel.cpp \
    src/tableinfo.cpp \
    src/tableinfomodel.cpp \
    src/dbanalyst.cpp \
    src/migrationtablematch.cpp
RESOURCES += resource/migraine.qrc

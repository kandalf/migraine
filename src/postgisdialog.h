#ifndef POSTGISDIALOG_H
#define POSTGISDIALOG_H

#include <QDialog>
#include "ui_postgisdialog.h"

class PostGISDialog : public QDialog, public Ui::PostGISDialog
{
Q_OBJECT
    public:
        PostGISDialog(QWidget *parent = 0, Qt::WFlags f = 0);

    protected:
        void setupObjectConnections();

    signals:
        void migrateGeometries(const bool &migrate);
        void migrateAsText(const bool &migrate);
        void ignoreGeometries(const bool &ignore);


};

#endif // POSTGISDIALOG_H

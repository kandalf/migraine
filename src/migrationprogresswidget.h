#ifndef MIGRATIONPROGRESSWIDGET_H
#define MIGRATIONPROGRESSWIDGET_H

#include <QFrame>
#include "ui_migrationprogresswidget.h"

class MigrationProgressWidget : public QFrame, public Ui::MigrationProgressWidget
{
Q_OBJECT
    public:
        MigrationProgressWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);

    public slots:
        void setCopyProgressTotal(const int &total);
        void setCopyProgress(const int &value);
        void setMigrateProgressTotal(const int &total);
        void setMigrateProgress(const int &value);
        void setCreateProgressTotal(const int &total);
        void setCreateProgress(const int &value);

};

#endif // MIGRATIONPROGRESSWIDGET_H

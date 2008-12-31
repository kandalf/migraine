#include "migrationprogresswidget.h"

MigrationProgressWidget::MigrationProgressWidget(QWidget *parent, Qt::WindowFlags f)
        :QFrame(parent, f)
{
    setupUi(this);
}

void MigrationProgressWidget::setCopyProgressTotal(const int &total)
{
    copyTablesProgressBar->setMaximum(total);
}

void MigrationProgressWidget::setCopyProgress(const int &value)
{
    copyTablesProgressBar->setValue(value);
}

void MigrationProgressWidget::setMigrateProgressTotal(const int &total)
{
    migrateTablesProgressBar->setMaximum(total);
}

void MigrationProgressWidget::setMigrateProgress(const int &value)
{
    migrateTablesProgressBar->setValue(value);
}

void MigrationProgressWidget::setCreateProgressTotal(const int &total)
{
    createTablesProgressBar->setMaximum(total);
}

void MigrationProgressWidget::setCreateProgress(const int &value)
{
    createTablesProgressBar->setValue(value);
}

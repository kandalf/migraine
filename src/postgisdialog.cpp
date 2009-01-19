#include "postgisdialog.h"

PostGISDialog::PostGISDialog(QWidget *parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setupObjectConnections();
}

void PostGISDialog::setupObjectConnections()
{
    connect( migrateAsTextRadioButton, SIGNAL(toggled(bool)), this, SIGNAL(migrateAsText(bool)) );
    connect( migrateRadioButton, SIGNAL(toggled(bool)), this, SIGNAL(migrateGeometries(bool)) );
    connect( ignoreRadioButton, SIGNAL(toggled(bool)), this, SIGNAL(ignoreGeometries(bool)) );
}


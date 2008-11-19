#ifndef MAPTABLENAMEMATCHMODEL_H
#define MAPTABLENAMEMATCHMODEL_H

#include <QHash>
#include "treemodel.h"


class TreeItem;
class MigrationTableMatch;

class MapTableNameMatchModel : public TreeModel
{
    Q_OBJECT
    public:
        MapTableNameMatchModel(const QHash<QString, MigrationTableMatch*> &data, QObject *parent = 0);

    public slots:
        void addOrCreateTableMatch(MigrationTableMatch*);

    protected:
        void setUpModelData(const QHash<QString, MigrationTableMatch*> &data, TreeItem *parent);
        QHash<QString, MigrationTableMatch*> modelData;
};

#endif // MAPTABLENAMEMATCHMODEL_H

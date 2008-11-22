#include <QList>
#include <QVariant>
#include <QSqlField>
#include "maptablenamematchmodel.h"
#include "migrationtablematch.h"
#include "tableinfo.h"
#include "treeitem.h"

MapTableNameMatchModel::MapTableNameMatchModel(const QHash<QString, MigrationTableMatch*> &data, QObject *parent)
        :TreeModel(parent)
{
    QList<QVariant> rootData;
    rootData << tr("Table/Source") << tr("Target");

    rootItem = new TreeItem(rootData);
    setUpModelData(data, rootItem);
}


void MapTableNameMatchModel::setUpModelData(const QHash<QString, MigrationTableMatch*> &data, TreeItem *parent)
{
    MigrationTableMatch *current;

    for (int i = 0; i < data.keys().count(); i++)
    {
        current = static_cast<MigrationTableMatch*>(data.value(data.keys().at(i)));
        if (current)
        {
            QList<QVariant> tableData;
            tableData << current->source()->name() << "";
            TreeItem *tableItem = new TreeItem(tableData, parent);
            for (int j = 0; j < current->count(); j++)
            {
                QList<QVariant> fieldData;
                fieldData << current->getMatch(j).first.name() << current->getMatch(j).second.name();
                tableItem->appendChild(new TreeItem(fieldData, tableItem));
            }
            modelData[data.keys().at(i)] = current;
            parent->appendChild(tableItem);
        }
    }
}

void MapTableNameMatchModel::addOrCreateTableMatch(MigrationTableMatch *table)
{
    bool tableExists = false;

    for (int i = 0; i < rootItem->childCount(); i++)
    {
        if (rootItem->child(i)->data(0).toString() == table->source()->name())
        {
            tableExists = true;
            for (int j = rootItem->child(i)->childCount(); j < table->count(); j++)
            {
                QList<QVariant> fieldData;
                fieldData << table->getMatch(j).first.name() << table->getMatch(j).second.name();
                rootItem->child(i)->appendChild(new TreeItem(fieldData, rootItem->child(i)));
            }
        }
        else
        {
            tableExists = false;
        }
    }

    if (!tableExists)
    {
        QHash<QString, MigrationTableMatch*> data;
        data[table->source()->name()] = table;
        setUpModelData(data, rootItem);
    }

//    setUpModelData(modelData, rootItem);
    emit(layoutChanged());
}

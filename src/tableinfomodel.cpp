#include <QListIterator>
#include <QVariant>

#include "tableinfomodel.h"
#include "treeitem.h"
#include "tableinfo.h"

TableInfoModel::TableInfoModel(const QList<TableInfo*> &data, QObject *parent)
		:TreeModel(parent)
{
	modelData = data;
	QList<QVariant> rootData;
	
	rootData << tr("Tables");
	rootItem = new TreeItem(rootData);
	setupModelData(modelData, rootItem);
}

void TableInfoModel::setupModelData(const QList<TableInfo*> &data, TreeItem *parent)
{
	for (int i = 0; i < data.count(); i++)
	{
		QList<QVariant> itemData;
		itemData << data.at(i)->name();
		TreeItem *tableItem = new TreeItem(itemData, parent);
		for (int j = 0; j < static_cast<TableInfo*>(data.at(i))->fieldNames().count(); j++)
		{
			QList<QVariant> field;
			//qDebug(data.at(i)->fieldTypes().at(j).toAscii());
			field << data.at(i)->fieldNames().at(j) << data.at(i)->fieldTypes().at(j);
			tableItem->appendChild(new TreeItem(field, tableItem));
		}
		parent->appendChild(tableItem);
	}
}

#ifndef __TABLEINFOMODEL_H__
#define __TABLEINFOMODEL_H__

#include <QList>
#include "treemodel.h"

class TableInfo;
class TreeItem;

class TableInfoModel : public TreeModel
{
	public:
		TableInfoModel(const QList<TableInfo*> &data, QObject *parent = 0);
	
	protected:
		void setupModelData(const QList<TableInfo*> &data, TreeItem *parent);
		QList<TableInfo*> modelData;
};

#endif // __TABLEINFOMODEL_H__

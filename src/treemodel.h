#ifndef __TREEMODEL_H__
#define __TREEMODEL_H__

#include <QAbstractItemModel>

class TreeItem;

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT
	public:
		TreeModel(const QString &data, QObject *parent = 0);
		~TreeModel();
		
		QVariant data(const QModelIndex &index, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		
	protected:
		virtual void setupModelData(const QStringList &data, TreeItem *parent);
		TreeItem *rootItem;
};

#endif // __TREEMODEL_H__

#ifndef DBANALYST_H
#define DBANALYST_H

#include <QObject>
#include <QList>
#include <QHash>

class TableInfo;
class MigrationTableMatch;

class DBAnalyst : public QObject
{
Q_OBJECT
    public:
        DBAnalyst(QObject *parent = 0);
        DBAnalyst(const QList<TableInfo*>&src, const QList<TableInfo*>&tgt, QObject *parent = 0);
        ~DBAnalyst();

    public slots:
        void setSourceList(const QList<TableInfo*>&list);
        void setTargetList(const QList<TableInfo*>&list);
        void analyzeDatabases();
        void analyzeDatabases(const QList<TableInfo*>&src, const QList<TableInfo*>&tgt);
        MigrationTableMatch *getNameMatchTable(const QString&name);
        void setTableMatch(const QString &tableName, const QString &src, const QString &tgt);

   signals:
        void exactMatchFound(const QString &name);
        void nameMatchFound(const QString &name);
        void noMatchFound(const QString &name);

    private:
        QList<TableInfo*> srcList;
        QList<TableInfo*> tgtList;
        QHash<QString, TableInfo*> exactMatches;
        QHash<QString, MigrationTableMatch*> nameMatches;
};

#endif // DBANALYST_H

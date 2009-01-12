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
Q_PROPERTY(bool createTables READ createTables WRITE setCreateTables);

    public:
        DBAnalyst(QObject *parent = 0);
        DBAnalyst(const QList<TableInfo*>&src, const QList<TableInfo*>&tgt, QObject *parent = 0);
        ~DBAnalyst();
        QStringList exactMatches() const;
        QStringList nameMatches() const;
        QStringList noMatches() const;
        QStringList tablesToMigrate() const;
        TableInfo *exactMatch(const QString &tableName) const;
        MigrationTableMatch *nameMatch(const QString &tableName) const;
        TableInfo *noMatch(const QString &tableName) const;

    public slots:
        void setSourceList(const QList<TableInfo*>&list);
        void setTargetList(const QList<TableInfo*>&list);
        void analyzeDatabases();
        void analyzeDatabases(const QList<TableInfo*>&src, const QList<TableInfo*>&tgt);
        MigrationTableMatch *getNameMatchTable(const QString&name);
        void setTableMatch(const QString &tableName, const QString &src, const QString &tgt);
        bool createTables() const;
        void setCreateTables(const bool &create);
        void reset();

   signals:
        void exactMatchFound(const QString &name);
        void nameMatchFound(const QString &name);
        void noMatchFound(const QString &name);
        void setMatchError(const QString &msg);
        void analysisDone(const bool &noError);

    private slots:
        bool isExactMatch(TableInfo *src, TableInfo *tgt);

    private:
        QList<TableInfo*> srcList;
        QList<TableInfo*> tgtList;
        QHash<QString, TableInfo*> _exactMatches;
        QHash<QString, MigrationTableMatch*> _nameMatches;
        QHash<QString, TableInfo*> _noMatches;
        bool _createTables;

};

#endif // DBANALYST_H

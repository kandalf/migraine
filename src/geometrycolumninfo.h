#ifndef GEOMETRYCOLUMNINFO_H
#define GEOMETRYCOLUMNINFO_H

#include <QString>

class GeometryColumnInfo
{
    public:
        GeometryColumnInfo(const QString &table = 0, const QString &column = 0, const QString &type = 0, const int &SRID = 0, const int &dimension = 0);
        QString tableName() const;
        QString columnName() const;
        QString type() const;
        int SRID() const;
        int dimension() const;

        void setTableName(const QString &name);
        void setColumnName(const QString &name);
        void setType(const QString &type);
        void setSRID(const int &id);
        void setDimension(const int &dimension);

    private:
        QString _tableName;
        QString _columnName;
        QString _geometryType;
        int _SRID;
        int _dimension;

};

#endif // GEOMETRYCOLUMNINFO_H

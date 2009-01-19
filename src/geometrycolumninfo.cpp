#include "geometrycolumninfo.h"

GeometryColumnInfo::GeometryColumnInfo(const QString &table, const QString &column, const QString &type, const int &SRID, const int &dimension)
{
    setTableName(table);
    setColumnName(column);
    setType(type);
    setSRID(SRID);
    setDimension(dimension);
}

void GeometryColumnInfo::setTableName(const QString &name)
{
    this->_tableName = name;
}

void GeometryColumnInfo::setColumnName(const QString &name)
{
    this->_columnName = name;
}

void GeometryColumnInfo::setType(const QString &type)
{
    this->_geometryType = type;
}

void GeometryColumnInfo::setSRID(const int &id)
{
    this->_SRID = id;
}

void GeometryColumnInfo::setDimension(const int &dimension)
{
    this->_dimension = dimension;
}

QString GeometryColumnInfo::tableName() const
{
    return this->_tableName;
}

QString GeometryColumnInfo::columnName() const
{
    return this->_columnName;
}

QString GeometryColumnInfo::type() const
{
    return this->_geometryType;
}

int GeometryColumnInfo::SRID() const
{
    return this->_SRID;
}

int GeometryColumnInfo::dimension() const
{
    return this->_dimension;
}

/**************************************************************************
**
** This file is part of Nut project.
** https://github.com/HamedMasafi/Nut
**
** Nut is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Nut is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Nut.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#include "mysqlgenerator.h"
#include "tablemodel.h"

NUT_BEGIN_NAMESPACE

MySqlGenerator::MySqlGenerator(Database *parent) : SqlGeneratorBase(parent)
{

}

QString MySqlGenerator::fieldType(FieldModel *field)
{
    QString dbType;

    switch (field->type) {
    case QVariant::Bool:
        dbType = "boolean";
        break;
    case QVariant::ByteArray:
        dbType = "blob";
        break;
    case QVariant::DateTime:
        dbType = "datetime";
        break;

    case QVariant::Date:
        dbType = "date";
        break;

    case QVariant::Time:
        dbType = "time";
        break;
    case QVariant::Double:
        dbType = "real";
        break;
    case QVariant::Int:
        dbType = "int(4)";
        if(field->isAutoIncrement)
            dbType += " auto_increment";

        break;
    case QVariant::String:
        if(field->length)
            dbType = QString("varchar(%1)").arg(field->length);
        else
            dbType = "text";
        break;

    case QVariant::Point:
    case QVariant::PointF:
        dbType = "POINT";
        break;

    case QVariant::Polygon:
    case QVariant::PolygonF:
        dbType = "POLYGON";
        break;

    default:
        qWarning("Type %s::%s(%d) is not supported",
                 qPrintable(field->name),
                 QMetaType::typeName(field->type),
                 field->type);
        dbType = "";
    }

    if(field->typeName == QStringLiteral("Nut::DbGeography"))
        dbType = "GEOMETRY";

    return dbType;
}

NUT_END_NAMESPACE

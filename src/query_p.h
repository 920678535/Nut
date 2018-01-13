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

#ifndef QUERY_P_H
#define QUERY_P_H

#include "wherephrase.h"

#include <QList>
#include <QString>

NUT_BEGIN_NAMESPACE

class Database;
class TableSetBase;
class QueryBase;
class RelationModel;
class QueryPrivate{
    QueryBase *q_ptr;
    Q_DECLARE_PUBLIC(QueryBase)

public:
    QueryPrivate(QueryBase *parent);
    ~QueryPrivate();

    QString sql;
    QString className;
    QString tableName;
    QString select;
    Database *database;
    TableSetBase *tableSet;
    QString joinClassName;
    QStringList joins;
    QList<RelationModel*> relations;
    QList<WherePhrase> wheres;
    QList<WherePhrase> orderPhrases;
    QHash<QString, QString> orders;
    int skip;
    int take;
};

NUT_END_NAMESPACE

#endif // QUERY_P_H

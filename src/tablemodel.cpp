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

#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QDebug>

#include <QJsonArray>
#include <QJsonObject>

#include "tablemodel.h"
#include "defines.h"

NUT_BEGIN_NAMESPACE

QSet<TableModel*> TableModel::_allModels;
//QMap<int, TableScheema*> TableScheema::scheemas;

QString TableModel::name() const
{
    return _name;
}

void TableModel::setName(const QString &name)
{
    _name = name;
}

QString TableModel::className() const
{
    return _className;
}

void TableModel::setClassName(const QString &className)
{
    _className = className;
}

int TableModel::typeId() const
{
    return _typeId;
}

void TableModel::setTypeId(const int &typeId)
{
    _typeId = typeId;
}

FieldModel *TableModel::field(QString name) const
{
    foreach (FieldModel *f, _fields)
        if(f->name == name)
            return f;
    
    return 0;
}

QList<FieldModel *> TableModel::fields() const
{
    return _fields;
}

QList<RelationModel *> TableModel::foregionKeys() const
{
    return _foregionKeys;
}

QStringList TableModel::fieldsNames() const
{
    QStringList ret;
    foreach (FieldModel *f, _fields)
        ret.append(f->name);
    return ret;
}

QSet<TableModel *> TableModel::allModels()
{
    return _allModels;
}

TableModel *TableModel::findByTypeId(int typeId)
{
    foreach (TableModel *model, _allModels)
        if(model->typeId() == typeId)
            return model;
    return 0;
}

//TableModel *TableModel::findByName(QString name)
//{
//    foreach (TableModel *model, _allModels)
//        if(model->name() == name)
//            return model;
//    return 0;
//}

TableModel *TableModel::findByClassName(QString className)
{
    foreach (TableModel *model, _allModels){
        if(model->className() == className)
            return model;
    }
    return 0;
}

bool TableModel::operator ==(const TableModel &t) const{
    if(_name != t.name())
        return false;

    if(fields().count() != t.fields().count())
        return false;

    foreach (FieldModel *f, _fields) {
        FieldModel *tf = t.field(f->name);
        if(!tf)
            return false;

        if(*f != *tf)
            return false;
    }

    return true;
}

bool TableModel::operator !=(const TableModel &t) const
{
    return !(*this == t);
}

bool TableModel::checkClassInfo(const QMetaClassInfo &classInfo,
                                QString &type, QString &name, QString &value)
{
    if (!QString(classInfo.name()).startsWith(__nut_NAME_PERFIX)) {
        return false;
    } else {
        QStringList parts = QString(classInfo.value()).split("\n");
        if (parts.count() != 3)
            return false;

        type = parts[0];
        name = parts[1];
        value = parts[2];
        return true;
    }
}

TableModel::TableModel(int typeId, QString tableName)
{
    //TODO: check that
//    if  (findByTypeId(typeId))
//        return;

    const QMetaObject *tableMetaObject = QMetaType::metaObjectForType(typeId);

    _typeId = typeId;
    _name = tableName;
    _className = tableMetaObject->className();

//#ifdef NUT_NAMESPACE
//    if(_className.startsWith(QT_STRINGIFY(NUT_NAMESPACE) "::"))
//        _className = _className.replace(QT_STRINGIFY(NUT_NAMESPACE) "::", "");
//#endif

    // get fields names
    for(int j = 0; j < tableMetaObject->classInfoCount(); j++){
        QString type;
        QString name;
        QString value;

        if (!checkClassInfo(tableMetaObject->classInfo(j),
                            type, name, value)) {
            continue;
        }

        if(type == __nut_FIELD){
            FieldModel *f = new FieldModel;
            f->name = name;
            _fields.append(f);
        }
    }
    // Browse all fields
    for(int j = 1; j < tableMetaObject->propertyCount(); j++){
        QMetaProperty fieldProperty = tableMetaObject->property(j);

        FieldModel *fieldObj = field(fieldProperty.name());
        foreach (FieldModel *f, _fields)
            if(f->name == fieldProperty.name())
                f = fieldObj;
        if(!fieldObj)
            continue;
        fieldObj->type = fieldProperty.type();
        fieldObj->typeName = QString(fieldProperty.typeName());
    }

    // Browse class infos
    for(int j = 0; j < tableMetaObject->classInfoCount(); j++){
        QString type;
        QString name;
        QString value;

        if (!checkClassInfo(tableMetaObject->classInfo(j),
                            type, name, value)) {
            continue;
        }

        if(type == __nut_FOREGION_KEY){
            RelationModel *fk = new RelationModel;
            fk->slaveTable = this;
            fk->localColumn = name;
            fk->foregionColumn = value;
            fk->masterClassName = value;
            _foregionKeys.append(fk);
        }

        if(type == __nut_FIELD){

        }


        FieldModel *f = field(name);
        if(!f)
            continue;

        if(type == __nut_LEN)
            f->length = value.toInt();
        else if(type == __nut_NOT_NULL)
            f->notNull = true;
        else if(type == __nut_DEFAULT_VALUE)
            f->defaultValue = value;
        else if(type == __nut_PRIMARY_KEY)
            f->isPrimaryKey = true;
        else if(type == __nut_AUTO_INCREMENT)
            f->isAutoIncrement = true;
        else if(type == __nut_UNIQUE)
            f->isUnique = true;


    }

    if(!findByTypeId(typeId) && !tableName.isNull())
        _allModels.insert(this);
}

/*
 "comment": {
        "auto_increment": "id",
        "fields": {
            "id": {
                "name": "id",
                "type": "int"
            },
            "userId": {
                "name": "userId",
                "type": "int"
            }
        },
        "primary_key": "id"
    },
*/
TableModel::TableModel(QJsonObject json, QString tableName)
{
    _name = tableName;

    QJsonObject fields = json.value(__FIELDS).toObject();
    foreach (QString key, fields.keys()) {
        QJsonObject fieldObject = fields.value(key).toObject();
        FieldModel *f = new FieldModel;
        f->name = fieldObject.value(__NAME).toString();
        f->type = QVariant::nameToType(fieldObject.value(__TYPE).toString().toLatin1().data());

        if(fieldObject.contains(__nut_NOT_NULL))
            f->notNull = fieldObject.value(__nut_NOT_NULL).toBool();

        if(fieldObject.contains(__nut_LEN))
            f->length = fieldObject.value(__nut_LEN).toInt();

        if(fieldObject.contains(__nut_DEFAULT_VALUE))
            f->defaultValue = fieldObject.value(__nut_DEFAULT_VALUE).toString();
        _fields.append(f);
    }

    if(json.keys().contains(__nut_AUTO_INCREMENT))
        field(json.value(__nut_AUTO_INCREMENT).toString())->isAutoIncrement = true;

    if(json.keys().contains(__nut_PRIMARY_KEY))
        field(json.value(__nut_PRIMARY_KEY).toString())->isAutoIncrement = true;

    _allModels.insert(this);
}

QJsonObject TableModel::toJson() const
{
    QJsonObject obj;
    QJsonObject fieldsObj;

    foreach (FieldModel *f, _fields) {
        QJsonObject fieldObj;
        fieldObj.insert(__NAME, f->name);
        fieldObj.insert(__TYPE, QString(QVariant::typeToName(f->type)));

        if(f->length)
            fieldObj.insert(__nut_LEN, f->length);

        if(f->notNull)
            fieldObj.insert(__nut_NOT_NULL, f->notNull);

        if(!f->defaultValue.isNull())
            fieldObj.insert(__nut_DEFAULT_VALUE, f->defaultValue);

        fieldsObj.insert(f->name, fieldObj);

        if(f->isAutoIncrement)
            obj.insert(__nut_PRIMARY_KEY, f->name);

        if(f->isPrimaryKey)
            obj.insert(__nut_AUTO_INCREMENT, f->name);
    }
    obj.insert(__FIELDS, fieldsObj);

    return obj;
}

//TableScheema *TableScheema::registerTable(int typeId, QString tableName)
//{
//    TableScheema *scheema = new TableScheema(typeId, tableName);
//    scheemas.insert(typeId, scheema);
//    return scheema;
//}

//void TableScheema::createForegionKeys()
//{
//    foreach (TableScheema *sch, scheemas) {
//        foreach (ForegionKey *fk, sch->_foregionKeys) {
//            fk->table = scheema(fk->tableName);
//        }
//    }
//}

//TableModel *TableModel::model(QString className)
//{
//    qFatal("");
//#ifdef NUT_NAMESPACE
//    if(className.startsWith(QT_STRINGIFY(NUT_NAMESPACE) "::"))
//        className = className.replace(QT_STRINGIFY(NUT_NAMESPACE) "::", "");
//#endif

//    foreach (TableModel *s, _allModels)
//        if(s->_className == className){
//            return s;
//        }
//    return 0;
//}

RelationModel *TableModel::foregionKey(QString otherTable) const
{
    foreach (RelationModel *fk, _foregionKeys)
        if(fk->masterClassName == otherTable)
            return fk;

    return 0;
}

QString TableModel::toString() const
{
    QStringList sl;
    foreach (FieldModel *f, _fields)
        sl.append(f->name + " " + QVariant::typeToName(f->type));

    QString ret = QString("%1 (%2)")
            .arg(_name)
            .arg(sl.join(", "));
    return ret;
}

QString TableModel::primaryKey() const
{
    foreach (FieldModel *f, _fields)
        if(f->isPrimaryKey)
            return f->name;
    return QString::null;
}

NUT_END_NAMESPACE

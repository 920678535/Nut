#ifndef USER_H
#define USER_H

#include "table.h"
#include "tableset.h"

#include <QtCore/QUuid>
#include <QtCore/QString>

#ifdef NUT_NAMESPACE
using namespace NUT_NAMESPACE;
#endif

class Comment;
class User : public Nut::Table
{
    Q_OBJECT

    NUT_PRIMARY_AUTO_INCREMENT(id)
    NUT_DECLARE_FIELD(QUuid, id, id, setId)

    NUT_DECLARE_FIELD(QString, username, username, setUsername)
    NUT_DECLARE_FIELD(QString, password, password, setPassword)

    NUT_DECLARE_CHILD_TABLE(Comment, comments)
public:
    Q_INVOKABLE User(QObject *tableSet = 0);
};

Q_DECLARE_METATYPE(User*)

#endif // USER_H

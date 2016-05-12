#ifndef COMMENT_H
#define COMMENT_H

#include <QtCore/qglobal.h>
#include <QtCore/QDateTime>
#include "table.h"

class Post;
class Comment : public Table
{
    Q_OBJECT

    NUT_PRIMARY_AUTO_INCREMENT(id)
    NUT_DECLARE_FIELD(int, id, id, setId)
    NUT_DECLARE_FIELD(QString, message, message, setMessage)
    NUT_DECLARE_FIELD(QDateTime, saveDate, saveDate, setSaveDate)

    NUT_FOREGION_KEY(Post, int, post, post, setPost)

public:
    Q_INVOKABLE explicit Comment(QObject *tableSet = 0);
};

#endif // COMMENT_H

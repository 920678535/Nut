#include <QtTest>
#include <QJsonDocument>
#include <QSqlError>

#include "maintest.h"
#include "query.h"
#include "tableset.h"
#include "tablemodel.h"
#include "databasemodel.h"

#include "post.h"
#include "comment.h"

MainTest::MainTest(QObject *parent) : QObject(parent)
{

}

void MainTest::initTestCase()
{
//    qDebug() << "Table type id:" << qRegisterMetaType<Table*>();
    qDebug() << "User type id:" << qRegisterMetaType<Post*>();
    qDebug() << "Comment type id:" << qRegisterMetaType<Comment*>();
    qDebug() << "DB type id:" << qRegisterMetaType<WeblogDatabase*>();

    //sql server
//    db.setDriver("QODBC");
//    db.setHostName("127.0.0.1");
//    db.setDatabaseName("DRIVER={SQL Server};Server=.;Database=Nut;Uid=sa;Port=1433;Pwd=qwe123!@#;WSID=.");
//    db.setUserName("sa");
//    db.setPassword("qwe123!@#");

    QStringList list;
    list << "one" << "two" << "three";
    FieldPhrase q = (Post::idField() = 1)
            & (Post::saveDateField() = QDateTime::currentDateTime())
            & (Post::saveDateField() < QDateTime::currentDateTime()
//            (/*(Post::saveDateField() > Post::idField())
//                     && */
//                     !Post::saveDateField().isNull()
//                     &&
//                      !Post::idField().in(list)
//                     || (Post::idField() == 4)
//                     && Post::saveDateField() >= QDateTime::currentDateTime()
                     /*|| Post::saveDateField().isNull()*/);
    qDebug() << "Command="<< q.command(0);
    QTEST_ASSERT(1==2);
    // postgres
    db.setDriver("QPSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("nutdb3");
    db.setUserName("postgres");
    db.setPassword("856856");

// mysql
//    db.setDriver("QMYSQL");
//    db.setHostName("127.0.0.1");
//    db.setDatabaseName("nutdb");
//    db.setUserName("root");
//    db.setPassword("onlyonlyi");

    bool ok = db.open();

    QTEST_ASSERT(ok);
}

void MainTest::dataScheema()
{
    auto json = db.model().toJson();
    auto model = DatabaseModel::fromJson(json);

    //    qDebug() << model.toJson();
    //    qDebug() << db.model().toJson();
    QTEST_ASSERT(model == db.model());
}

void MainTest::createPost()
{
    Post *newPost = new Post;
    newPost->setTitle("post title");
    newPost->setSaveDate(QDateTime::currentDateTime());

    db.posts()->append(newPost);

    for(int i = 0 ; i < 3; i++){
        Comment *comment = new Comment;
        comment->setMessage("comment #" + QString::number(i));
        comment->setSaveDate(QDateTime::currentDateTime());
        newPost->comments()->append(comment);
    }
    db.saveChanges();

    postId = newPost->id();

    QTEST_ASSERT(newPost->id() != 0);
    qDebug() << "New post inserted with id:" << newPost->id();
}

void MainTest::selectPosts()
{
    q = FROM(db.posts())
        JOIN(Comment)
        WHERE(Post::idField() == postId);

    auto posts = q->toList();

    post = posts.at(0);
    post->setBody("");

    QTEST_ASSERT(posts.length() == 1);
    QTEST_ASSERT(posts.at(0)->comments()->length() == 3);
    QTEST_ASSERT(posts.at(0)->title() == "post title");

    QTEST_ASSERT(posts.at(0)->comments()->at(0)->message() == "comment #0");
    QTEST_ASSERT(posts.at(0)->comments()->at(1)->message() == "comment #1");
    QTEST_ASSERT(posts.at(0)->comments()->at(2)->message() == "comment #2");
    db.cleanUp();
}

void MainTest::testDate()
{
    QDateTime d = QDateTime::currentDateTime();
    QTime t = QTime(d.time().hour(), d.time().minute(), d.time().second());
    d.setTime(t);

    Post *newPost = new Post;
    newPost->setTitle("post title");
    newPost->setSaveDate(d);

    db.posts()->append(newPost);

    db.saveChanges();

    auto q = FROM(db.posts())
            WHERE(Post::idField() == newPost->id())
            FIRST();

    qDebug() << d << q->saveDate();
    QTEST_ASSERT(q->saveDate() == d);
}


void MainTest::selectWithInvalidRelation()
{
    auto q = FROM(db.posts())
            JOIN(Invalid_Class_Name)
            SELECT();
}

void MainTest::modifyPost()
{
    auto q = FROM(db.posts())
            WHERE(Post::idField() == postId);

    Post *post = q->first();

    QTEST_ASSERT(post != 0);

    post->setTitle("new name");
    db.saveChanges();

    q = FROM(db.posts())
            WHERE(Post::idField() == postId);

    post = q->first();
    QTEST_ASSERT(post->title() == "new name");
}

void MainTest::deletePost()
{
    auto count = FROM(db.posts())
            WHERE(Post::idField() == postId)
            DELETE();

    QTEST_ASSERT(count == 1);

    count = FROM(db.posts())
            WHERE(Post::idField() == postId)
            COUNT();

    QTEST_ASSERT(count == 0);
}

QTEST_MAIN(MainTest)

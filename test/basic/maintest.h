#ifndef MAINTEST_H
#define MAINTEST_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>

#include "weblogdatabase.h"
class Post;
class User;
class MainTest : public QObject
{
    Q_OBJECT
    WeblogDatabase db;
    int postId;
    Post *post;
    User *user;

public:
    explicit MainTest(QObject *parent = 0);

signals:

private slots:
    void initTestCase();

    void dataScheema();
    void join();
    void createUser();
    void createPost();
    void createPost2();
    void selectPosts();
    void selectFirst();
    void selectPostsWithoutTitle();
    void selectPostIds();
    void testDate();
    void selectWithInvalidRelation();
    void select10NewstPosts();
    void modifyPost();
    void emptyDatabase();
};

#endif // MAINTEST_H

#include <QApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "mainwindow.h"
#include "login.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    qDebug() << "testDebug";
    /*
    qDebug()<<QSslSocket::supportsSsl();
    qDebug()<<QSslSocket::sslLibraryBuildVersionString();
    */

    login l;
    l.show();
    MainWindow w;
    QObject::connect(&l,SIGNAL(showmain(QString,QString)),&w,SLOT(receivelogin(QString,QString)));

    return a.exec();
}

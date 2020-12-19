#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "testDebug";
    /*
    qDebug()<<QSslSocket::supportsSsl();
    qDebug()<<QSslSocket::sslLibraryBuildVersionString();
    */
    MainWindow w;
    w.show();
    return a.exec();

}

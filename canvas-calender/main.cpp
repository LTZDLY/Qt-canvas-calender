#include <QApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
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

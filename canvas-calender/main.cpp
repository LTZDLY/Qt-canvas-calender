#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "login.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    login l;
    l.show();
    MainWindow w;
    QObject::connect(&l, SIGNAL(showmain(QString, QString)), &w,
                     SLOT(receivelogin(QString, QString)));

    return a.exec();
}

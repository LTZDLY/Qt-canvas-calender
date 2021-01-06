#ifndef LOGIN_H
#define LOGIN_H

#include <qabstractbutton.h>

#include <QDialog>

#include "mainwindow.h"

namespace Ui {
class login;
}

class login : public QDialog {
    Q_OBJECT

   public:
    explicit login(QWidget *parent = nullptr);
    ~login();

   signals:
    void showmain(QString, QString);

   private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

   private:
    Ui::login *ui;

    QNetworkAccessManager *manager;  //新建session
};

#endif  // LOGIN_H

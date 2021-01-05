#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qjsondocument.h>

#include <QFile>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkSession>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void set_user_ID(QString id){
        this->Ecom_User_ID=id;
    }
    void set_passwd(QString pswd){
        this->Ecom_Password=pswd;
    }

   private slots:
    //void on_pushButton_clicked();

    void on_calendarWidget_clicked(const QDate &date);

    void on_pushButton_add_clicked();

    void on_pushButton_del_clicked();

   private:
    Ui::MainWindow *ui;

    QNetworkAccessManager *manager;  //新建session

    QByteArray token = "";
    QByteArray user = "user_234063";
    QByteArray post(QString url, QByteArray data, const char *type);
    QByteArray get(QString url);

    QJsonObject json;       //所登录用户的所有ddl信息
    QString Ecom_User_ID;
    QString Ecom_Password;
    QJsonArray selected_day_json;



    void updatetoken(QNetworkReply *reply);
    QJsonObject format(QByteArray data);

private slots:
    void receivelogin(QString,QString);
    void on_comboBox_currentIndexChanged(int index);
};
#endif  // MAINWINDOW_H

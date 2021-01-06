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

   private slots:
    void on_calendarWidget_clicked(const QDate &date);
    void on_pushButton_add_clicked();
    void on_pushButton_del_clicked();

   private:
    Ui::MainWindow *ui;

    QNetworkAccessManager *manager;  //新建session

    QByteArray token = "";
    QByteArray user = "";
    QByteArray name = "";

    QJsonObject json;  //所登录用户的所有ddl信息
    QJsonArray selected_day_json;
    QString select_date;

    QByteArray post(QString url, QByteArray data, const char *type);
    QByteArray get(QString url);
    QJsonObject format(QByteArray data);
    void updatetoken(QNetworkReply *reply);

   private slots:
    void receivelogin(QString, QString);
    void on_comboBox_currentIndexChanged(int index);
};
#endif  // MAINWINDOW_H

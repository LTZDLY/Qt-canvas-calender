#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <qjsondocument.h>
#include <QFile>
#include <QNetworkCookie>
#include <QLineEdit>
#include <QInputDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QString tjGetCode();
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void readData(QNetworkReply *reply);

    void on_pushButton_clicked();

    void on_calendarWidget_clicked(const QDate &date);

    void on_pushButton_add_clicked();

private:
    QJsonParseError jsonError;
    QJsonDocument document;
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;

};
#endif // MAINWINDOW_H

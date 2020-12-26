#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete manager;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QNetworkRequest requestInfo;    //设置请求信息
    QEventLoop eventLoop;           //建立阻断事件
    QNetworkReply *reply;           //存储返回信息
    QByteArray responseByte;        //存储返回内容
    QVariant statusCode;            //状态码
    QJsonParseError jsonError;
    QJsonDocument document;

    //设置https协议
    /*
    QSslConfiguration m_sslConfig = QSslConfiguration::defaultConfiguration();
    m_sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    m_sslConfig.setProtocol(QSsl::TlsV1_2);
    requestInfo.setSslConfiguration(m_sslConfig);
    */

    /*
    //添加事件循环机制，返回后再运行后面的
    requestInfo.setUrl(QUrl("https://ids.tongji.edu.cn:8443/nidp/app/login?flag=true"));
    reply = manager->get(requestInfo);                                                  //获取信息
    QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));      //等待事件完成
    eventLoop.exec();                                                                   //在事件完成前阻断
    responseByte = reply->readAll();                                                    //读取信息
    */

    responseByte = get("https://ids.tongji.edu.cn:8443/nidp/app/login?flag=true");

    //对获取到的信息封装为json对象并传入自动验证码解析网站
    QJsonDocument m_httpDocum;
    QJsonObject data;
    QByteArray  m_httpData;
    data.insert("data_url", QString::fromUtf8(responseByte));
    m_httpDocum.setObject(data);
    m_httpData = m_httpDocum.toJson(QJsonDocument::Compact);

    /*
    requestInfo.setUrl(QUrl("http://172.81.215.215/pi/crack"));
    requestInfo.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json")); //设置数据格式
    reply = manager->post(requestInfo, m_httpData);                                         //发送post请求
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseByte = reply->readAll();
    */

    responseByte = post("http://172.81.215.215/pi/crack", m_httpData, "application/json");

    document = QJsonDocument::fromJson(responseByte, &jsonError);
    QJsonObject object = document.object();
    QJsonValue value = object.value("ans");
    QString Ecom_code = value.toString();
    QString Ecom_User_ID = "";
    QString Ecom_Password = "";
    QByteArray b;
    b.append("option=credential");
    b.append("&Ecom_User_ID=" + Ecom_User_ID.toUtf8());
    b.append("&Ecom_Password="+Ecom_Password.toUtf8());
    b.append("&Ecom_code=" + Ecom_code.toUtf8());

    post("https://ids.tongji.edu.cn:8443/nidp/app/login", b, "application/x-www-form-urlencoded");


    //该处链接跳转会发生4次重定向，暂时不清楚较为妥当的化简方式故不打算化简
    requestInfo.setUrl(QUrl("http://canvas.tongji.edu.cn/login"));
    reply = manager->get(requestInfo);
    QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    while (statusCode == 301 || statusCode == 302){
        //状态码为301或302，发生重定向:
        QVariant redirectionTargetUrl =reply->attribute(QNetworkRequest::RedirectionTargetAttribute);   //获取重定向的地址
        requestInfo.setUrl(QUrl(redirectionTargetUrl.toString()));                                      //设置重定向的地址
        //qDebug() << "redirection Url is " << redirectionTargetUrl.toString();
        reply = manager->get(requestInfo);
        QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();
        updatetoken(reply);
        statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    }

    /*
    requestInfo.setUrl(QUrl("http://canvas.tongji.edu.cn/api/v1/planner/items?start_date=2020-12-14T16:00:00.000Z"));
    reply = manager->get(requestInfo);
    QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseByte = reply->readAll();
    */
    responseByte = get("http://canvas.tongji.edu.cn/api/v1/planner/items?start_date=2020-12-14T16:00:00.000Z");
    qDebug() << responseByte;
}

void MainWindow::readData(QNetworkReply *reply){
    qDebug()<< "data\n";
    QByteArray array = reply->readAll();
    qDebug()<< "array:" <<array;

    /*
    document = QJsonDocument::fromJson(array,&jsonError);
    if(!document.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();
            QJsonValue value = object.value("data");
            QJsonArray array = value.toArray();
            int nSize = array.size();
            for(int i = 0; i < nSize; ++i)
            {
                QJsonValue object = array.at(i);
                if(object.isObject())
                    qDebug()<<object;
            }
        }
    }
    */
}

void MainWindow::on_calendarWidget_clicked(const QDate &date)
{

}

void MainWindow::on_pushButton_add_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("输入事件"),tr("请输入事件名称"), QLineEdit::Normal,0, &ok);
    if (!ok || text.isEmpty()) return;

    auto date = ui->calendarWidget->selectedDate();
    qDebug() << text.toUtf8();
    ui->textEdit->setText(text);
    QString event_title = text;
    QString event_start = "";
    QString event_end = "";
    QString event_local = "";
    QString _method = "POST";
    date.setDate(date.year(), date.month(), date.day() - 1);
    event_start = event_end = date.toString("yyyy-MM-ddT16%3A00%3A00Z");


    QByteArray byte;
    byte.append("calendar_event%5Btitle%5D=" + event_title.toUtf8());
    byte.append("&calendar_event%5Bstart_at%5D=" + event_start.toUtf8());
    byte.append("&calendar_event%5Bend_at%5D=" + event_end.toUtf8());
    byte.append("&calendar_event%5Blocation_name%5D=" + event_local.toUtf8());
    byte.append("&calendar_event%5Bcontext_code%5D=" + user);
    byte.append("&authenticity_token=" + token);
    QString url = "http://canvas.tongji.edu.cn/api/v1/calendar_events";
    post(url, byte, "application/x-www-form-urlencoded");

}

QByteArray MainWindow::post(QString url, QByteArray data, const char* type) {

    QNetworkRequest requestInfo;
    QEventLoop eventLoop;
    QNetworkReply *reply;
    QByteArray responseByte;

    requestInfo.setUrl(QUrl(url));              //设置url
    requestInfo.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(type));      //设置post的数据类型
    requestInfo.setHeader(QNetworkRequest::ContentLengthHeader, data.size());       //设置post的数据长度

    reply = manager->post(requestInfo, data);   //发送post请求
    QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));  //等待事件完成
    eventLoop.exec();       //在事件完成前阻断

    updatetoken(reply);     //更新token

    return reply->readAll();
}

QByteArray MainWindow::get(QString url){

    QNetworkRequest requestInfo;
    QEventLoop eventLoop;
    QNetworkReply *reply;
    QByteArray responseByte;

    requestInfo.setUrl(QUrl(url));      //设置url
    reply = manager->get(requestInfo);  //发送get请求
    QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));  //等待事件完成
    eventLoop.exec();                   //在事件完成前阻断

    return reply->readAll();
}

void MainWindow::on_pushButton_del_clicked()
{
    /*
    auto c = QMessageBox::question(this, tr("删除ddl"),QString(tr("确认删除ddl?")),QMessageBox::Yes | QMessageBox::No);
    if (c == QMessageBox::No) return;

    QByteArray byte;

    byte.append("calendar_event%5Btitle%5D=" + event_title.toUtf8());
    byte.append("&calendar_event%5Bstart_at%5D=" + event_start.toUtf8());
    byte.append("&calendar_event%5Bend_at%5D=" + event_end.toUtf8());
    byte.append("&calendar_event%5Blocation_name%5D=" + event_local.toUtf8());
    byte.append("&calendar_event%5Bcontext_code%5D=user_234063");
    byte.append("&authenticity_token=" + token.toUtf8());

    byte = "authenticity_token=asDk6ktTfaVnKEiyKXo5lAqKvXWADTdoRnjjy9TiAek4q9WFHh8T3AFNI8FbVU%2FgYcPJJ6smWD9pU5WPpNFKqA%3D%3D&_method=DELETE";

    QString url = "http://canvas.tongji.edu.cn/api/v1/calendar_events/1923";
    post(url, byte);
    */

}

void MainWindow::updatetoken(QNetworkReply* reply) {
    if(reply->hasRawHeader("Set-Cookie")) {
        QVariant variantCookies = reply->header(QNetworkRequest::SetCookieHeader);
        QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie> >(variantCookies);
        for (auto&& i : cookies)
            if(i.name() == "_csrf_token") {
                token = i.value();
                break;
            }
    }
}

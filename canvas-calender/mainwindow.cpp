#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow() {
    delete manager;
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    QNetworkRequest requestInfo;  //设置请求信息
    QEventLoop eventLoop;         //建立阻断事件
    QNetworkReply *reply;         //存储返回信息
    QByteArray responseByte;      //存储返回内容
    QVariant statusCode;          //状态码
    QJsonParseError jsonError;
    QJsonDocument document;

    responseByte =
        get("https://ids.tongji.edu.cn:8443/nidp/app/login?flag=true");

    //对获取到的信息封装为json对象并传入自动验证码解析网站
    QJsonDocument m_httpDocum;
    QJsonObject data;
    QByteArray m_httpData;
    data.insert("data_url", QString::fromUtf8(responseByte));
    m_httpDocum.setObject(data);
    m_httpData = m_httpDocum.toJson(QJsonDocument::Compact);

    responseByte =
        post("http://172.81.215.215/pi/crack", m_httpData, "application/json");

    document = QJsonDocument::fromJson(responseByte, &jsonError);
    QJsonObject object = document.object();
    QJsonValue value = object.value("ans");
    QString Ecom_code = value.toString();
    QString Ecom_User_ID = "";
    QString Ecom_Password = "";
    QByteArray b;
    b.append("option=credential");
    b.append("&Ecom_User_ID=" + Ecom_User_ID.toUtf8());
    b.append("&Ecom_Password=" + Ecom_Password.toUtf8());
    b.append("&Ecom_code=" + Ecom_code.toUtf8());

    post("https://ids.tongji.edu.cn:8443/nidp/app/login", b,
         "application/x-www-form-urlencoded");

    //该处链接跳转会发生4次重定向，暂时不清楚较为妥当的化简方式故不打算化简
    requestInfo.setUrl(QUrl("http://canvas.tongji.edu.cn/login"));
    reply = manager->get(requestInfo);
    QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop,
                             SLOT(quit()));
    eventLoop.exec();
    statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    while (statusCode == 301 || statusCode == 302) {
        //状态码为301或302，发生重定向:
        QVariant redirectionTargetUrl = reply->attribute(
            QNetworkRequest::RedirectionTargetAttribute);  //获取重定向的地址
        requestInfo.setUrl(
            QUrl(redirectionTargetUrl.toString()));  //设置重定向的地址
        // qDebug() << "redirection Url is " << redirectionTargetUrl.toString();
        reply = manager->get(requestInfo);
        QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop,
                                 SLOT(quit()));
        eventLoop.exec();
        updatetoken(reply);
        statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    }

    responseByte =
        get("http://canvas.tongji.edu.cn");
    qDebug() << responseByte;

    responseByte =
        get("http://canvas.tongji.edu.cn/api/v1/planner/"
            "items?start_date=2020-01-01T16:00:00.000Z&per_page=100");
    qDebug() << responseByte;
    this->json = format(responseByte);
}

void MainWindow::on_calendarWidget_clicked(const QDate &date) {
    qDebug() << date.toString("yyyy-MM-dd");
    auto a = this->json[date.toString("yyyy-MM-dd")].toArray();
    qDebug() << a;
}

void MainWindow::on_pushButton_add_clicked() {
    bool ok;
    QString text = QInputDialog::getText(
        this, tr("输入事件"), tr("请输入事件名称"), QLineEdit::Normal, 0, &ok);
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

QByteArray MainWindow::post(QString url, QByteArray data, const char *type) {
    QNetworkRequest requestInfo;
    QEventLoop eventLoop;
    QNetworkReply *reply;
    QByteArray responseByte;

    requestInfo.setUrl(QUrl(url));  //设置url
    requestInfo.setHeader(QNetworkRequest::ContentTypeHeader,
                          QVariant(type));  //设置post的数据类型
    requestInfo.setHeader(QNetworkRequest::ContentLengthHeader,
                          data.size());  //设置post的数据长度

    reply = manager->post(requestInfo, data);  //发送post请求
    QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop,
                             SLOT(quit()));  //等待事件完成
    eventLoop.exec();                        //在事件完成前阻断

    updatetoken(reply);  //更新token

    return reply->readAll();
}

QByteArray MainWindow::get(QString url) {
    QNetworkRequest requestInfo;
    QEventLoop eventLoop;
    QNetworkReply *reply;
    QByteArray responseByte;

    requestInfo.setUrl(QUrl(url));      //设置url
    reply = manager->get(requestInfo);  //发送get请求
    QNetworkSession::connect(reply, SIGNAL(finished()), &eventLoop,
                             SLOT(quit()));  //等待事件完成
    eventLoop.exec();                        //在事件完成前阻断

    updatetoken(reply);  //更新token

    return reply->readAll();
}

void MainWindow::on_pushButton_del_clicked() {
    /*
    auto c = QMessageBox::question(this,
    tr("删除ddl"),QString(tr("确认删除ddl?")),QMessageBox::Yes |
    QMessageBox::No); if (c == QMessageBox::No) return;

    QByteArray byte;

    byte.append("calendar_event%5Btitle%5D=" + event_title.toUtf8());
    byte.append("&calendar_event%5Bstart_at%5D=" + event_start.toUtf8());
    byte.append("&calendar_event%5Bend_at%5D=" + event_end.toUtf8());
    byte.append("&calendar_event%5Blocation_name%5D=" + event_local.toUtf8());
    byte.append("&calendar_event%5Bcontext_code%5D=user_234063");
    byte.append("&authenticity_token=" + token.toUtf8());

    byte =
    "authenticity_token=asDk6ktTfaVnKEiyKXo5lAqKvXWADTdoRnjjy9TiAek4q9WFHh8T3AFNI8FbVU%2FgYcPJJ6smWD9pU5WPpNFKqA%3D%3D&_method=DELETE";

    QString url = "http://canvas.tongji.edu.cn/api/v1/calendar_events/1923";
    post(url, byte);
    */
}

void MainWindow::updatetoken(QNetworkReply *reply) {
    if (reply->hasRawHeader("Set-Cookie")) {
        QVariant variantCookies =
            reply->header(QNetworkRequest::SetCookieHeader);
        QList<QNetworkCookie> cookies =
            qvariant_cast<QList<QNetworkCookie> >(variantCookies);
        for (auto &&i : cookies)
            if (i.name() == "_csrf_token") {
                token = i.value();
                break;
            }
    }
}

QJsonObject MainWindow::format(QByteArray data) {
    data.remove(0, 9);
    QJsonDocument json_doc = QJsonDocument::fromJson(data);
    QJsonObject json;
    QMap<QString, QJsonArray> m;
    QVariantList list = json_doc.toVariant().toList();
    for(int i = 0; i < list.count(); i++) {
        QJsonObject temp;
        QVariantMap map = list[i].toMap();
        auto time = QDateTime::fromString(map["plannable_date"].toString(), "yyyy-MM-ddThh:mm:ssZ");
        time.setTimeSpec(Qt::UTC);
        QDateTime localTime = time.toLocalTime();
        QString date = localTime.toString("yyyy-MM-dd");
        qDebug() << date;
        temp = temp.fromVariantMap(map);
        if (m.contains(date))
            m[date].append(temp);
        else {
            QJsonArray t;
            t.append(temp);
            m[date] = t;
        }
    }
    for (auto i = m.begin(); i != m.end(); i++) {
        qDebug() << i.key();
        qDebug() << i.value();
        json.insert(i.key(), i.value());
    }
    return json;
}

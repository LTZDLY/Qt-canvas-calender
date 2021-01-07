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

void MainWindow::receivelogin(QString id, QString pswd) {
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

    QByteArray b;
    b.append("option=credential");
    b.append("&Ecom_User_ID=" + id.toUtf8());
    b.append("&Ecom_Password=" + pswd.toUtf8());
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
        get("http://canvas.tongji.edu.cn/api/v1/planner/"
            "items?start_date=2020-01-01T16:00:00.000Z&per_page=100");
    QString s = responseByte;
    if (s.contains("errors")) {
        QMessageBox::critical(
            this, tr("错误"),
            QString(tr("登录失败，请关闭程序并检查输入的学号密码是否正确。")),
            QMessageBox::Ok);
            QApplication* app;
            app->exit(0);
    } else {
        this->json = format(responseByte);
        responseByte = get("http://canvas.tongji.edu.cn");
        QString html = responseByte;
        QStringList list = html.split("current_user\":{\"id\":\"");
        QStringList listsub = list[1].split("\",\"display_name\":\"");
        QStringList listssub = listsub[1].split("\",\"avatar_image_url");
        user = listsub[0].toUtf8();
        name = listssub[0].toUtf8();
    }
    this->show();
}

void MainWindow::on_calendarWidget_clicked(const QDate &date) {
    this->select_date = date.toString("yyyy-MM-dd");
    this->selected_day_json = this->json[date.toString("yyyy-MM-dd")].toArray();
    if (selected_day_json.size()) {
        ui->comboBox->clear();
        for (int i = 0; i < selected_day_json.size(); i++) {
            auto context_name =
                selected_day_json[i].toObject()["context_name"].toString();
            ui->comboBox->addItem(context_name);
        }
    } else {
        ui->comboBox->clear();
        ui->label_ddl->clear();
        ui->label_title->clear();
    }
}

void MainWindow::on_pushButton_add_clicked() {
    bool ok;
    QString text = QInputDialog::getText(
        this, tr("输入事件"), tr("请输入事件名称"), QLineEdit::Normal, 0, &ok);
    if (!ok || text.isEmpty()) return;

    auto date = ui->calendarWidget->selectedDate();

    QString event_title = text;
    QString event_start = "";
    QString event_end = "";
    QString event_local = "";
    QString _method = "POST";
    date = date.addDays(-1);
    event_start = event_end = date.toString("yyyy-MM-ddT16%3A00%3A00Z");

    QByteArray byte;
    byte.append("calendar_event%5Btitle%5D=" + event_title.toUtf8());
    byte.append("&calendar_event%5Bstart_at%5D=" + event_start.toUtf8());
    byte.append("&calendar_event%5Bend_at%5D=" + event_end.toUtf8());
    byte.append("&calendar_event%5Blocation_name%5D=" + event_local.toUtf8());
    byte.append("&calendar_event%5Bcontext_code%5D=user_" + user);
    byte.append("&_method=POST");
    byte.append("&authenticity_token=" + token);
    QString url = "http://canvas.tongji.edu.cn/api/v1/calendar_events";
    auto re = post(url, byte, "application/x-www-form-urlencoded");

    auto document = QJsonDocument::fromJson(re);
    auto object = document.object();
    auto value = object.value("id");
    auto plid = value.toInt();

    QJsonObject temp;
    temp.insert("context_name", QString(name));
    temp.insert("context_type", "User");
    QString d = date.toString("yyyy-MM-dd");
    d = d + "T16:00:00Z";
    temp.insert("plannable_date", d);
    QJsonObject pl;
    pl.insert("title", text);
    temp.insert("plannable", QJsonValue(pl));
    temp.insert("plannable_id", plid);
    QJsonValueRef ref = this->json.find(this->select_date).value();
    QJsonArray arr = ref.toArray();
    arr.append(temp);
    ref = arr;
    on_calendarWidget_clicked(ui->calendarWidget->selectedDate());
}

void MainWindow::on_pushButton_del_clicked() {
    auto index = ui->comboBox->currentIndex();
    if (this->selected_day_json.size() == 0) return;
    if (selected_day_json[index].toObject()["context_type"].toString() !=
        "User") {
        QMessageBox::critical(this, tr("错误"),
                              QString(tr("此为课程ddl，无法被删除")),
                              QMessageBox::Ok);
    } else {
        auto c = QMessageBox::question(this, tr("删除ddl"),
                                       QString(tr("确认删除ddl?")),
                                       QMessageBox::Yes | QMessageBox::No);
        if (c == QMessageBox::No) return;

        QString plannable_id = QString::number(this->selected_day_json[index]
                                                   .toObject()["plannable_id"]
                                                   .toDouble());

        QString url = "http://canvas.tongji.edu.cn/api/v1/calendar_events/" +
                      plannable_id;
        QByteArray byte;
        byte.append("_method=DELETE");
        byte.append("&authenticity_token=" + token);
        post(url, byte, "application/x-www-form-urlencoded");

        QJsonValueRef ref = json.find(this->select_date).value();
        QJsonArray arr = ref.toArray();
        arr.removeAt(index);
        ref = arr;
        this->selected_day_json = this->json[this->select_date].toArray();
    }
    on_calendarWidget_clicked(ui->calendarWidget->selectedDate());
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
    for (int i = 0; i < list.count(); i++) {
        QJsonObject temp;
        QVariantMap map = list[i].toMap();
        auto time = QDateTime::fromString(map["plannable_date"].toString(),
                                          "yyyy-MM-ddThh:mm:ssZ");
        time.setTimeSpec(Qt::UTC);
        QDateTime localTime = time.toLocalTime();
        QString date = localTime.toString("yyyy-MM-dd");
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
        json.insert(i.key(), i.value());
    }
    return json;
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

void MainWindow::on_comboBox_currentIndexChanged(int index) {
    if (index >= 0) {
        auto title = this->selected_day_json[index]
                         .toObject()["plannable"]
                         .toObject()["title"]
                         .toString();
        ui->label_title->setText(title);
        auto ddl_time = this->selected_day_json[index]
                            .toObject()["plannable_date"]
                            .toString();
        auto time = QDateTime::fromString(ddl_time, "yyyy-MM-ddThh:mm:ssZ");
        time.setTimeSpec(Qt::UTC);
        QDateTime ddl = time.toLocalTime();
        ui->label_ddl->setText(ddl.toString("yyyy-MM-dd hh:mm:ss"));
    }
}

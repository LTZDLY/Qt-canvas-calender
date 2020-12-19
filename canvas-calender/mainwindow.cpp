#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);

    connect(manager,&QNetworkAccessManager::finished,this,&MainWindow::readData);
}

MainWindow::~MainWindow()
{
    delete ui;
}
QString MainWindow::tjGetCode(){
    QNetworkAccessManager *m_pHttpMgr = new QNetworkAccessManager();
    //设置url
    QString url = "https://ids.tongji.edu.cn:8443/nidp/app/login?flag=true";//fae4b325e52c
    QNetworkRequest requestInfo;
    requestInfo.setUrl(QUrl(url));

    //添加事件循环机制，返回后再运行后面的
    QEventLoop eventLoop;
    QNetworkReply *reply =  m_pHttpMgr->get(requestInfo);
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();       //block until finish
    //错误处理
    /**/
    //请求返回的结果
    QByteArray responseByte = reply->readAll();
    qDebug() << responseByte;

    QJsonDocument m_httpDocum;
    QJsonObject data;
    QByteArray  m_httpData;

    data.insert("data_url", QString::fromUtf8(responseByte));
    m_httpDocum.setObject(data);
    m_httpData = m_httpDocum.toJson(QJsonDocument::Compact);


    url = "http://172.81.215.215/pi/crack";
    requestInfo.setUrl(QUrl(url));
    requestInfo.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    reply = m_pHttpMgr->post(requestInfo, m_httpData);
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseByte = reply->readAll();
    qDebug() << responseByte;
    document = QJsonDocument::fromJson(responseByte,&jsonError);
    QJsonObject object = document.object();
    QJsonValue value = object.value("ans");
    return value.toString();

}

void MainWindow::on_pushButton_clicked()
{

    QNetworkAccessManager *m_pHttpMgr = new QNetworkAccessManager();
    //设置url
    QString url = "https://ids.tongji.edu.cn:8443/nidp/app/login?flag=true";//fae4b325e52c
    QNetworkRequest requestInfo;
    QEventLoop eventLoop;
    QNetworkReply *reply;
    QByteArray responseByte;

    requestInfo.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    QSslConfiguration m_sslConfig = QSslConfiguration::defaultConfiguration();
    m_sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    m_sslConfig.setProtocol(QSsl::TlsV1_2);
    requestInfo.setSslConfiguration(m_sslConfig);

    //添加事件循环机制，返回后再运行后面的
    requestInfo.setUrl(QUrl(url));
    reply = m_pHttpMgr->get(requestInfo);
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();       //block until finish
    //错误处理
    //请求返回的结果

    QString cookie = "JSESSIONID=FC06D6558A59A6024153B036FB3A57AA; UrnNovellNidpClusterMemberId=~03~0Bslo~0A~0B~14mop~09~0E";
    if(reply->hasRawHeader("Set-Cookie"))
    {
        //cookie = reply->rawHeader("Set-Cookie");
        qDebug() << cookie;
    }
    QByteArray cookieByte = cookie.toUtf8();

    QList<QNetworkCookie> list;
    list.push_back(QNetworkCookie(cookieByte));
    QVariant var;
    var.setValue(list);


    responseByte = reply->readAll();

    QJsonDocument m_httpDocum;
    QJsonObject data;
    QByteArray  m_httpData;

    data.insert("data_url", QString::fromUtf8(responseByte));
    m_httpDocum.setObject(data);
    m_httpData = m_httpDocum.toJson(QJsonDocument::Compact);


    url = "http://172.81.215.215/pi/crack";
    requestInfo.setUrl(QUrl(url));
    requestInfo.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    reply = m_pHttpMgr->post(requestInfo, m_httpData);
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseByte = reply->readAll();
    qDebug() << responseByte;
    document = QJsonDocument::fromJson(responseByte,&jsonError);
    QJsonObject object = document.object();
    QJsonValue value = object.value("ans");
    QString Ecom_code = value.toString();

    QJsonObject datan;
    QString Ecom_User_ID = "";
    QString Ecom_Password = "";
    datan.insert("option", "credential");
    datan.insert("Ecom_User_ID", Ecom_User_ID);
    datan.insert("Ecom_Password", Ecom_Password);
    datan.insert("Ecom_code", "jrqg");
    QJsonDocument m_httpDocumn;
    QByteArray m_httpDatan = m_httpDocum.toJson(QJsonDocument::Compact);
    m_httpDocumn.setObject(datan);
    m_httpDatan = m_httpDocumn.toJson(QJsonDocument::Compact);


    //添加事件循环机制，返回后再运行后面的
    requestInfo.setUrl(QUrl("https://ids.tongji.edu.cn:8443/nidp/app/login"));
    requestInfo.setHeader(QNetworkRequest::CookieHeader, var);
    reply = m_pHttpMgr->post(requestInfo, m_httpDatan);
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    responseByte = reply->readAll();
    ui->textEdit->setText(responseByte);
    qDebug() << responseByte;

    if(reply->hasRawHeader("Set-Cookie"))
    {
        QByteArray cookie = reply->rawHeader("Set-Cookie");
        qDebug() << QString::fromUtf8(cookie);
    }
    requestInfo.setUrl(QUrl("http://canvas.tongji.edu.cn/login"));
    reply = m_pHttpMgr->get(requestInfo);
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    responseByte = reply->readAll();
    qDebug() << responseByte;

    if(reply->hasRawHeader("Set-Cookie"))
    {
        QByteArray cookie = reply->rawHeader("Set-Cookie");
        qDebug() << QString::fromUtf8(cookie);
    }

    /*
    QString cookie = "SRV=4bdedc14-fcd6-43b7-b99c-59ad94969975; log_session_id=12b0559cfc3cf5a798af21c581ee8cb7; _csrf_token=Gtv15iaJQM0VyAm5dFVFm7iMNsyKQf9TT6o1ho99IgFC8LqEYrkBiUaHOt4BPxfawM1HmLg1lRAO7nTIyQ9PTQ%3D%3D; _normandy_session=RO8cXOP_wrPSXBgYFMVlbg+D3fMtpi-aafcGngehjA9epu4jZmKv8y6uoiX3NeP8kZpjOUTsu89P7fwCYJICFHNHTFfZ1H_YApIxJ1N-gU2MrUoEWNl0uXBnl467UOI24wjviDBZhwAJ50QfO_Jr6hz4Ye_WBvp1-7sNKhTS-kfzNkmPY6fSM9DlLnlrrZVR553z7MiCtd3Jlxdgw0g1uRsNtsPE3acrh8rdjpC4OjLegeCJpCJhScM7Ld09nhYyxhyL5GardfIzYgtJDI-KyA1Vegq9ttnTTeSKMR_ZfZXHfkvXaTFvTu2qNIDO_0CjaboPmGU9JlNnG-PtZvhIvNQXlozc17BcXh7HayUUA7eTvslKBP7R1DjcEySzggIj2kgDEBTIbVnKxv2JIVJDqi3.fI_GRBdFIeWOT62WDKu2veGNXEc.X9i3Rg";
    QByteArray cookieByte = cookie.toUtf8();
    QList<QNetworkCookie> list;
    list.push_back(QNetworkCookie(cookieByte));
    QVariant var;
    var.setValue(list);
    requestInfo.setHeader(QNetworkRequest::CookieHeader, var);
    */

    requestInfo.setUrl(QUrl("http://canvas.tongji.edu.cn/api/v1/planner/items?start_date=2020-12-14T16:00:00.000Z"));
    reply = m_pHttpMgr->get(requestInfo);
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseByte = reply->readAll();
    qDebug() << responseByte;

    /*
    QString addr = "https://ids.tongji.edu.cn:8443/nidp/app/login?flag=true";
    qDebug()<<addr;
    QUrl url(addr);
    QNetworkRequest request(url);
    manager->get(request);
    */
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
    if (ok && !text.isEmpty()){}
}

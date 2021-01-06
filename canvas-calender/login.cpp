#include "login.h"

#include "ui_login.h"

login::login(QWidget* parent) : QDialog(parent), ui(new Ui::login) {
    ui->setupUi(this);
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
}

login::~login() { delete ui; }

void login::on_pushButton_clicked() {
    this->hide();
    emit showmain(ui->lineEdit_id->text(), ui->lineEdit_password->text());
}

void login::on_pushButton_2_clicked() {
    QApplication* app;
    app->exit(0);
}

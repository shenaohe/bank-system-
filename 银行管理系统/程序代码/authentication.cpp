#include "authentication.h"
#include "ui_authentication.h"
#include "QCryptographicHash"
#include <QMessageBox>
authentication::authentication(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::authentication)
{
    ui->setupUi(this);
    ui->pwd->setEchoMode(QLineEdit::Password);
    this->setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

authentication::~authentication()
{
    delete ui;
}

void authentication::on_exitButton_clicked()
{
    ui->cardid->clear();
    ui->pwd->clear();
    login=false;
    this->close();
}

void authentication::on_okButton_clicked()
{
    QString pwd=ui->pwd->text();//卡密码
    QCryptographicHash MD5(QCryptographicHash::Md5);
    MD5.addData(pwd.toLatin1().data());
    QString hashpwd=MD5.result().toHex();
    QString sqlpwd="";
    //首先选择卡类别
    QString cardtype=ui->cardtype->currentText();
    QString cardid=ui->cardid->text();
    if(cardid!=""&&pwd!=""){
        QString sqls=QString("select pwd from authenticate where card_type='%1' and card_id='%2';").arg(cardtype).arg(cardid);
        S->query->exec(sqls);
        while(S->query->next()){
            sqlpwd=S->query->value(0).toString();
        }
        //判断
        if(hashpwd==sqlpwd){
            card_id=cardid;//卡号
            card_type=cardtype;
            QMessageBox::information(this, "提示", "验证成功",QMessageBox::Ok);
            login=true;
            ui->cardid->setText("");
            ui->pwd->setText("");
            this->close();
        }
        else {
            QMessageBox::information(this, "提示", "验证失败",QMessageBox::Close);
        }
    }
    else {
        QMessageBox::warning(this, "错误", "请输入用户名和密码",QMessageBox::Close);
    }
}

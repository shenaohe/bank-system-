#include "admindlg.h"
#include "ui_admindlg.h"
#include"global_var.h"
#include"QMessageBox"
#include "QCryptographicHash"
admindlg::admindlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::admindlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

}

admindlg::~admindlg()
{
    delete ui;
}

void admindlg::on_pushButton_clicked()
{
    QString id=ui->lineEdit->text();
    QString pwd=ui->lineEdit_2->text();
    if(pwd!=""&&id!=""){
        QString sqlpwd="";
        QString sqls=QString("select pwd from admin where id='%1';").arg(id);
        QCryptographicHash MD5(QCryptographicHash::Md5);
        MD5.addData(pwd.toLatin1().data());
        QString hashpwd=MD5.result().toHex();
        S->query->exec(sqls);
        while(S->query->next()){
            sqlpwd=S->query->value(0).toString();
        }
        if(sqlpwd==hashpwd){
            adminlogin=true;
            this->close();
        }
        else {
            QMessageBox::critical(this,"错误","账号或密码错误",QMessageBox::Cancel);
        }
    }
    else {
        QMessageBox::warning(this,"错误","请输入账号和密码",QMessageBox::Cancel);
    }
}

void admindlg::on_pushButton_2_clicked()
{
    this->close();
    adminlogin=false;
}

#include "deleteaccount.h"
#include "ui_deleteaccount.h"
#include"global_var.h"
#include "QCryptographicHash"
deleteaccount::deleteaccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deleteaccount)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
}

deleteaccount::~deleteaccount()
{
    delete ui;
}

void deleteaccount::on_xiaohu_clicked()
{
    QString name=ui->name->text();
    QString id=ui->ID->text();
    QString cardid=ui->cardid->text();
    QString pwd=ui->pwd->text();
    QString cardtype=ui->comboBox->currentText();

    if(ui->yue->text().toInt()==0){
        //销户,在数据库中删除所有与之有关的信息
        //默认正常情况，即用户没有贷款，没有透支
        QString sqls1="";
        QString sqls2="";
        QString sqls3="";
        //client
        sqls1=QString("delete from client where client_id='%1' and card_type='%2' and card_id='%3';").
                arg(id).arg(cardtype).arg(cardid);
        //card_boc
        sqls2=QString("delete from card_boc where card_type='%1' and card_id='%2';").arg(cardtype).arg(cardid);

        //authenticate
        sqls3=QString("delete from authenticate where card_type='%1' and card_id='%2';").arg(cardtype).arg(cardid);

        if(S->DeletefromSQL(sqls1)&&S->DeletefromSQL(sqls2)&&S->DeletefromSQL(sqls3)){
            QMessageBox::information(this,"提示","删除成功",QMessageBox::Ok);
            this->close();
        }
        else {
            QMessageBox::information(this,"提示","删除失败",QMessageBox::Ok);
        }
    }
    else {
        QMessageBox::warning(this,"警告","请取出所有余额",QMessageBox::Ok);
    }
}

void deleteaccount::on_find_clicked()
{
    QString name=ui->name->text();
    QString id=ui->ID->text();
    QString cardid=ui->cardid->text();
    QString pwd=ui->pwd->text();
    QString cardtype=ui->comboBox->currentText();
    //先认证
    QCryptographicHash MD5(QCryptographicHash::Md5);
    MD5.addData(pwd.toLatin1().data());
    QString hashpwd=MD5.result().toHex();
    QString sqlpwd="";
    QString sqlcardid="";
    QString sqlcardtype="";
    QString sqls=QString("select pwd from authenticate where card_id='%1' and card_type='%2';").arg(cardid).arg(cardtype);
    S->query->exec(sqls);
    while(S->query->next()){
        sqlpwd=S->query->value(0).toString();
    }
    //判断
    sqls=QString("select card_type,card_id from client where name='%1' and client_id='%2';").arg(name).arg(id);
    S->query->exec(sqls);
    while(S->query->next()){
        sqlcardtype=S->query->value(0).toString();
        sqlcardid=S->query->value(1).toString();
    }
    if(sqlcardid!=cardid||sqlcardtype!=cardtype){
        QMessageBox::question(this, "提示", "用户信息有误",QMessageBox::Close);
    }
    else {
        if(hashpwd==sqlpwd){
            //QMessageBox::question(this, "提示", "验证成功",QMessageBox::Ok);
            //显示余额
            sqls=QString("select overage from card_boc where card_id='%1' and card_type='%2';").arg(cardid).arg(cardtype);
            S->query->exec(sqls);
            while(S->query->next()){
                ui->yue->setText(S->query->value(0).toString());
            }
        }
        else {
            QMessageBox::question(this, "提示", "用户信息有误",QMessageBox::Close);
        }
    }
}

void deleteaccount::on_qukuan_clicked()
{
    //先取出所有余额
    int curentmoney=ui->yue->text().toInt();
    if(curentmoney>0){
        //取款
        QString sqls=QString("update card_boc set overage=0 where card_type='%1' and card_id='%2';").arg(card_type).arg(card_id);
        S->UpdatefromSQL(sqls);
    }
    ui->yue->setText("0");
}

void deleteaccount::on_quxiao_clicked()
{
    this->close();
}

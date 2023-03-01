#include "openaccount.h"
#include "ui_openaccount.h"
#include"banksql.h"
#include <QMessageBox>
#include"global_var.h"
#include"QDateTime"
#include"QTime"
#include"QDate"
#include "QCryptographicHash"
openaccount::openaccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::openaccount)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
}

openaccount::~openaccount()
{
    delete ui;
}

void openaccount::on_kaihu_clicked()
{ 
    //更新client表，创建新用户
    QList<QString> attribute;
    QString name=ui->name->text();
    QString id=ui->ID->text();
    QString cardtype=ui->comboBox->currentText();
    QString money=ui->initmoney->text();
    if(name!=""&&id!=""&&cardtype!=""&&money!=""){
        //判断初始金额是否大于10
        if(money<10){
            QMessageBox::warning(this,"警告","请存入至少10元",QMessageBox::Cancel);
        }
        else{//开户
            QString cardid="";
            QDate current_date=QDate::currentDate();//当前日期
            QString year=QString("%1").arg(current_date.year());//年
            QString month=QString("%1").arg(current_date.month());//月
            QString day=QString("%1").arg(current_date.day());//日
            QTime current_time = QTime::currentTime();//当前时间
            QString hour=QString("%1").arg(current_time.hour());//时
            QString minute=QString("%1").arg(current_time.minute());//分
            QString second=QString("%1").arg(current_time.second());//秒
            cardid=year+month+day+hour+minute+second;
            QString sqls=QString("select is_untrustable from overdraft where client_id='%1';").arg(id);
            S->query->exec(sqls);
            int istrust=-1;
            while(S->query->next()){
                istrust=S->query->value(0).toInt();
            }
            if(istrust==0){
                QMessageBox::warning(this,"警告","该用户当前不允许申请可透支卡",QMessageBox::Ok);
                attribute<<cardid<<cardtype<<money<<"0"<<"0"<<"1";
            }
            else{
                attribute<<cardid<<cardtype<<money<<"1"<<"0"<<"1";
            }
            if(S->AddtoSQL("card_boc",&attribute)){//新增用户表
                attribute.clear();
                attribute<<id<<name<<cardid<<cardtype<<"4";
                S->AddtoSQL("client",&attribute);//新增用户表
                QMessageBox::information(this,"提示","存款成功",QMessageBox::Ok);
                attribute.clear();
                QString pwd="123456";
                QCryptographicHash MD5(QCryptographicHash::Md5);
                MD5.addData(pwd.toLatin1().data());
                QString hashpwd=MD5.result().toHex();
                attribute<<cardid<<cardtype<<hashpwd;
                S->AddtoSQL("authenticate",&attribute);
                this->close();
            }
        }
    }
    else {
        QMessageBox::information(this,"提示","请输入对应的值",QMessageBox::Ok);
    }
}

void openaccount::on_quxiao_clicked()
{
    this->close();
}

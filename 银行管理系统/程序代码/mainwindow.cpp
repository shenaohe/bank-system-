#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"qdebug.h"
#include <QMessageBox>
#include<QDateTime>
#include"time.h"
#include <QFile>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);
    QString qss;
    QFile file(":/qss/flatwhite.css");
    if (file.open(QFile::ReadOnly))
    {
        //用readAll读取默认支持的是ANSI格式,如果不小心用creator打开编辑过了很可能打不开
        qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
    ui->stackedWidget->setCurrentIndex(0);
    ui->Screen->setCurrentIndex(0);
    ui->treeWidget->clear();
    ui->treeWidget->setIndentation(0);
    ui->treeWidget->setColumnCount(1);
    QStringList msg;
    msg.clear();
    msg<<"普通用户";
    QTreeWidgetItem *p1 = new QTreeWidgetItem(ui->treeWidget,msg);
    p1->setIcon(0,QIcon(":/picture/usericon.svg"));
    ui->treeWidget->addTopLevelItem(p1);
    msg.clear();
    msg<<"管理员";
    QTreeWidgetItem *p2 = new QTreeWidgetItem(ui->treeWidget,msg);
    p2->setIcon(0,QIcon(":/picture/adminicon.svg"));
    ui->treeWidget->addTopLevelItem(p2);

    ui->tableWidget_3->horizontalHeader()->setVisible(false);
    ui->tableWidget_3->setColumnWidth(0,500);
    Group = new QButtonGroup(this);
    Group->addButton(ui->radioButton,0);
    Group->addButton(ui->radioButton_2,1);
    connect(ui->radioButton,SIGNAL(clicked()),this,SLOT(PushButtonClick()));
    connect(ui->radioButton_2,SIGNAL(clicked()),this,SLOT(PushButtonClick()));
    //信誉度处理
    QDateTime sqldate;
    QList<QDateTime> mydate;
    QDateTime currentdate=QDateTime::currentDateTime();
    QStringList client_id;
    QStringList card_id;
    QStringList card_type;
    QString sqls=QString("select over_date,client_id,card_id,card_type from overdraft;");
    S->query->exec(sqls);
    while(S->query->next()){

        sqldate=QDateTime::fromString(S->query->value(0).toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"),"yyyy-MM-dd hh:mm:ss.zzz");
        mydate.append(sqldate);
        client_id<<S->query->value(1).toString();
        card_id<<S->query->value(2).toString();
        card_type<<S->query->value(3).toString();
    }
    for(int i=0;i<mydate.length();i++){
        if(mydate.at(i).addDays(15)<currentdate){
            //超出还款期限，设置信誉度为0
            sqls=QString("update client set credit=0 where client_id='%1' and card_id='%2' and card_type='%3';").arg(client_id[i]).arg(card_id[i]).arg(card_type[i]);
            S->query->exec(sqls);
            S->UpdatefromSQL(sqls);
            S->query->clear();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete Group;
}
void MainWindow::pagechange(QString pagename){
    //登录验证成功
    if(login==false){
        ui->Screen->setCurrentWidget(ui->screen_page1);
    }
    else{
        assert(login==true);
        if(pagename=="cunkuan"){
            ui->Screen->setCurrentWidget(ui->page_cunkuan);
        }
        else if (pagename=="qukuan") {
            ui->Screen->setCurrentWidget(ui->page_qukuan);
        }
        else if (pagename=="daikuan") {
            ui->Screen->setCurrentWidget(ui->page_daikuan);
        }
        else if(pagename=="zhuanzhang"){
            ui->Screen->setCurrentWidget(ui->page_zhuanzhang);
        }
        else if(pagename=="huandai"){
            ui->Screen->setCurrentWidget(ui->page_huandai);
        }
        else if(pagename=="yu_e"){
            ui->Screen->setCurrentWidget(ui->page_yu_e);
        }
    }
}
void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parent = item->parent();
    if(nullptr == parent){
        int index_row=ui->treeWidget->indexOfTopLevelItem(item);
        if(index_row==0&&column==0){
            ui->stackedWidget->setCurrentIndex(0);
            ui->identity->setText("普通用户");
        }
        else if (index_row==1&&column==0) {
            //管理员认证
            adminn.exec();
            if(adminlogin==true){
                ui->stackedWidget->setCurrentIndex(1);
                ui->identity->setText("管理员");
                adminlogin=false;
            }
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox::question(this, "attention", "you are exiting!",QMessageBox::Yes|QMessageBox::No);
    exit(0);
}

void MainWindow::on_cunkuan_clicked()
{
    authdlg.exec();
    pagechange("cunkuan");
    login=false;
}
void MainWindow::on_cunkuan_ok_clicked()
{
    //存款成功--更新数据库
    int money=ui->cunkun_money->text().toInt();//用户当前想要存款的金额
    QString sqls=QString("update card_boc set overage=overage+%1 where card_type='%2' and card_id='%3';")
            .arg(money).arg(card_type).arg(card_id);
    //透支处理
    /*
     * 1.判断当前用户是否具有透支金额
     * 2.
    */
    int overdraft=0;
    sqls=QString("select overdraft from card_boc where card_id='%1' and card_type='%2';").arg(card_id).arg(card_type);
    S->query->exec(sqls);
    while(S->query->next()){
        overdraft=S->query->value(0).toInt();
    }
    //具有透支金额
    if(overdraft>0){
        sqls=QString("select over_date from overdraft where card_id='%1' and card_type='%2';").arg(card_id).arg(card_type);
        S->query->exec(sqls);
        QDateTime sqldate;
        QDateTime currentdate=QDateTime::currentDateTime();
        while(S->query->next()){
          //sqldate=QDateTime::fromString(S->query->value(0).toDateTime().toString(),"yyyy-MM-dd hh:mm:ss.zzz");
          sqldate=QDateTime::fromString(S->query->value(0).toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"),"yyyy-MM-dd hh:mm:ss.zzz");
        }
        //if(sqldate.addDays(15)>=currentdate){
        //判断存款金额是否大于透支金额
        if(money>overdraft){//足够还清透支款项
            //卡中透支金额清零,从透支表中删除该项
            sqls=QString("update card_boc set overdraft=0 where card_id='%1' and card_type='%2';").arg(card_id).arg(card_type);
            if(!S->UpdatefromSQL(sqls)){
                QMessageBox::information(this, "提示", "更新失败!",QMessageBox::Ok);
                exit(0);
            }
            sqls=QString("delete from overdraft where card_id='%1' and card_type='%2';").arg(card_id).arg(card_type);
            if(!S->DeletefromSQL(sqls)){
                QMessageBox::information(this, "提示", "删除失败!",QMessageBox::Ok);
                exit(0);
            }
            //将剩余贷款存入
            int truecunkuanmoney=money-overdraft;
            sqls=QString("update card_boc set overage=overage+%1 where card_type='%2' and card_id='%3';")
                    .arg(truecunkuanmoney).arg(card_type).arg(card_id);
            if(S->UpdatefromSQL(sqls)){
                QMessageBox::information(this, "提示", "存款成功!",QMessageBox::Ok);
                card_id.clear();
                card_type.clear();
                //返回主界面
                ui->Screen->setCurrentWidget(ui->screen_page1);
                //在还款期限内
                if(sqldate.addDays(15)>=currentdate){
                    //客户信誉度+1
                    int credit=0;
                    sqls=QString("select credit from client where card_type='%1' and card_id='%2';").arg(card_id).arg(card_type);
                    S->query->exec(sqls);
                    while(S->query->next()){
                        credit=S->query->value(0).toInt();
                    }
                    if(credit<=3){
                        credit+=1;
                        sqls=QString("update client set credit=%1 where card_type='%2' and card_id='%3';").arg(credit).arg(card_id).arg(card_type);
                        S->UpdatefromSQL(sqls);
                    }
                }
                //超出还款期限
                else {
                    //设置信誉度=0
                    sqls=QString("update client set credit=%1 where card_type='%2' and card_id='%3';").arg(0).arg(card_id).arg(card_type);
                    S->UpdatefromSQL(sqls);
                }
            }
            else{
                QMessageBox::critical(this, "提示", "存款失败!",QMessageBox::Ok);
                exit(0);
            }
        }
        else {//不能还清透支款
            overdraft=overdraft-money;
            //更新卡表
            sqls=QString("update card_boc set overdraft=%1 where card_type='%2' and card_id='%3';").arg(overdraft).arg(card_type).arg(card_id);
            S->UpdatefromSQL(sqls);
            //更新透支表
            sqls=QString("update overdraft set over_bill=%1 where card_type='%2' and card_id='%3';").arg(overdraft).arg(card_type).arg(card_id);
            S->UpdatefromSQL(sqls);
            //在期限内，信誉度不变，仍为可信任
            //不在期限内，设置信誉度最低，不可信任
            sqls=QString("update client set credit=%1 where card_type='%2' and card_id='%3';").arg(0).arg(card_id).arg(card_type);
            S->UpdatefromSQL(sqls);
            sqls=QString("update  overdraft set is_untrustable=%1 where card_type='%2' and card_id='%3';").arg(0).arg(card_id).arg(card_type);
            S->UpdatefromSQL(sqls);
        }
    }
    //不具备透支金额，直接存款
    else{
        sqls=QString("update card_boc set overage=overage+%1 where card_type='%2' and card_id='%3';")
                    .arg(money).arg(card_type).arg(card_id);
        if(!S->UpdatefromSQL(sqls)){
            QMessageBox::information(this, "提示", "存款失败!",QMessageBox::Cancel);
            exit(0);
        }
        else {
            QMessageBox::information(this, "提示", "存款成功!",QMessageBox::Ok);
            card_id.clear();
            card_type.clear();
            ui->cunkun_money->clear();
            //返回主界面
            ui->Screen->setCurrentWidget(ui->screen_page1);
        }
    }
    S->query->clear();
}

void MainWindow::on_qukuan_clicked()
{
    authdlg.exec();
    pagechange("qukuan");
    login=false;
}

void MainWindow::on_qukuan_ok_clicked()
{
    int money=ui->qukuan_money->text().toInt();
    int SQLmoney=0;
    //查询余额
    QString sqls=QString("select overage from card_boc where card_type='%1' and card_id='%2';").arg(card_type).arg(card_id);
    S->query->exec(sqls);
    while(S->query->next()){
        SQLmoney=S->query->value(0).toInt();
    }
    //余额大于取款金额
    if(SQLmoney>=money){
        //更新数据库
        sqls=QString("update card_boc set overage=overage-%1 where card_type='%2' and card_id='%3';").arg(money).arg(card_type).arg(card_id);
        if(S->UpdatefromSQL(sqls)){
            QMessageBox::information(this, "提示", "取款成功!",QMessageBox::Ok);
        }
        else {
            QMessageBox::information(this, "Ops!", "取款失败!",QMessageBox::Ok);
        }
    }
    //余额小于取款金额
    else{
        //判断是否卡是否具有透支功能
        int overtype=0;
        int overmoney=0;//当前卡的透支金额
        sqls=QString("select over_type,overdraft from card_boc where card_type='%1' and card_id='%2';").arg(card_type).arg(card_id);
        S->query->exec(sqls);
        while(S->query->next()){
            overtype=S->query->value(0).toInt();
            overmoney=S->query->value(1).toInt();
        }
        QMap<int,int> creditmap;
        creditmap[0]=500;
        creditmap[1]=1000;
        creditmap[2]=1500;
        creditmap[3]=2500;
        creditmap[4]=5000;
        //具有透支功能
        if(overtype==1){
            //查询用户信誉度
            int credit=0;
            int maxmoney=0;
            sqls=QString("select credit from client where card_type='%1' and card_id='%2';").arg(card_type).arg(card_id);
            S->query->exec(sqls);
            while(S->query->next()){
                credit=S->query->value(0).toInt();
            }
            //最大透支金额
            maxmoney=creditmap[credit];
            //判断用户透支金额是否大于maxmoey
            int draft=money-SQLmoney;
            //在允许的透支金额内
            if(maxmoney>=overmoney+draft){
                //允许透支,更新数据库
                SQLmoney=0;
                overmoney=overmoney+draft;
                sqls=QString("update card_boc set overage=%1,overdraft=%2 where card_type='%3' and card_id='%4';")
                        .arg(SQLmoney).arg(overmoney).arg(card_type).arg(card_id);
                qDebug()<<sqls;
                S->UpdatefromSQL(sqls);
                //弹出提示
                QString msg=QString("取款成功,当前卡的透支金额为%1").arg(overmoney);
                QMessageBox::information(this, "提示", msg,QMessageBox::Ok);
                ui->qukuan_money->setText("0");
                //同步更新透支表--未实现
                //判断是否存在透支，存在则更新
                int count=0;
                sqls=QString("SELECT count(*) FROM overdraft where card_id='%1' and card_type='%2';").arg(card_id).arg(card_type);
                S->query->exec(sqls);
                while(S->query->next()){
                    count=S->query->value(0).toInt();
                }
                if(count>0){
                    sqls=QString("update overdraft set over_bill=%1 where card_id='%2' and card_type='%3';").arg(card_id).arg(card_type);
                    S->UpdatefromSQL(sqls);
                }
                //不存在则加入
                QString clientid="";
                sqls=QString("select client_id from client where card_id='%1' and card_type='%2';").arg(card_id).arg(card_type);
                S->query->exec(sqls);
                while(S->query->next()){
                    clientid=S->query->value(0).toString();
                }
                QDateTime current_time =QDateTime::currentDateTime();
                QString current_date_time = current_time.toString("yyyy-MM-dd hh:mm:ss");
                QList<QString> attribute;
                //不存在则加入
                attribute<<clientid<<card_id<<card_type<<QString("%1").arg(overmoney)<<QString("%1").arg(current_date_time)<<"1";
                S->AddtoSQL("overdraft",&attribute);
            }
            //超出了透支上限
            else{
                QString msg=QString("余额失败,您当前允许透支上限为%1,当前卡的透支金额为%2").arg(maxmoney).arg(overmoney);
                QMessageBox::information(this, "提示", msg,QMessageBox::Ok);
            }
        }
        //不具备透支功能
        else {
            QString msg=QString("取款失败,余额不足");
            QMessageBox::information(this, "提示", msg,QMessageBox::Cancel);
        }
    }
    //清空登录信息
    card_id.clear();
    card_type.clear();
    login=false;
    //返回主界面
    ui->Screen->setCurrentWidget(ui->screen_page1);
}

void MainWindow::on_daikuan_clicked()
{

    authdlg.exec();
    pagechange("daikuan");
    if(login==true){
        int loan_privilege=0;
        QMap<int,int> loanmap;
        loanmap[0]=0;
        loanmap[1]=100000;
        loanmap[2]=150000;
        loanmap[3]=250000;
        loanmap[4]=500000;
        QString sqls=QString("select loan_privileges from card_boc where card_type='%1' and card_id='%2';").arg(card_type).arg(card_id);
        S->query->exec(sqls);
        while(S->query->next()){
            loan_privilege=S->query->value(0).toInt();
        }
        int maxloan=loanmap[loan_privilege];
        QString msg=QString("%1").arg(maxloan);
        ui->maxmoneyshow->setText(msg);
    }
    login=false;
}

void MainWindow::on_daikuan_ok_clicked()
{
    int maxloan=ui->maxmoneyshow->text().toInt();
    int loanmoney=ui->loanmoney->text().toInt();
    //贷款小于最高金额,允许贷款生成合同书
    QString msg="";
    if(ui->loanmoney->text()!=""){
        if(loanmoney<=maxloan){
            //生成合同书
            QDateTime current_time =QDateTime::currentDateTime();
            QString current_date_time =current_time.toString("yyyy-MM-dd hh:mm:ss.zzz ddd");
            int repaymoney=(ui->comboBox_2->currentText().toInt())*365*int((ui->interest_rate->text().toDouble())*(ui->loanmoney->text().toDouble()));
            msg=QString("贷款额:%1\n贷款时间:%2\n贷款开始时间:%3\n贷款利率:%4\n应还贷款:%5\n贷款银行:%6")
                    .arg(ui->loanmoney->text())
                    .arg(ui->comboBox_2->currentText())
                    .arg(current_date_time)
                    .arg(ui->interest_rate->text())
                    .arg(repaymoney+loanmoney)
                    .arg("不存在的银行");
            ui->hetongshu->setText(msg);
        }
        else {
            msg="贷款的金额超过你的权限";
            QMessageBox::information(this, "提示",msg,QMessageBox::Cancel);
        }
    }
    else{
        QMessageBox::information(this, "提示","请填写贷款金额",QMessageBox::Cancel);
    }
}

void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->interest_rate->setText("0.001");
        break;
    case 1:
        ui->interest_rate->setText("0.002");
        break;
    case 2:
        ui->interest_rate->setText("0.003");
        break;
    case 3:
        ui->interest_rate->setText("0.004");
        break;
    case 4:
        ui->interest_rate->setText("0.005");
        break;
    case 5:
        ui->interest_rate->setText("0.006");
        break;
    case 6:
        ui->interest_rate->setText("0.007");
        break;
    case 7:
        ui->interest_rate->setText("0.008");
        break;
    }
}

void MainWindow::on_daikuan_start_clicked()
{
    int maxloan=ui->maxmoneyshow->text().toInt();
    int loanmoney=ui->loanmoney->text().toInt();
    //贷款小于最高金额,允许贷款
    QString msg="";
    QString sqls=QString("select count(*) from loan where card_type='%1' and card_id='%2';").arg(card_type).arg(card_id);
    S->query->exec(sqls);
    int exist=0;
    while(S->query->next()){
        exist=S->query->value(0).toInt();
    }
    //判断是否存在这个人
    if(exist==0){//未存在贷款
        if(ui->hetongshu->toPlainText()!=""){
            if(loanmoney<=maxloan){
                //确认读了合同书
                if(ui->checkbox_aggree->isChecked()){

                    //更新数据库
                    QDateTime current_time =QDateTime::currentDateTime();
                    QString current_date_time = current_time.toString("yyyy-MM-dd hh:mm:ss");
                    QList<QString> attribute;
                    QString clientid="";
                    sqls=QString("select client_id from  client where card_type='%1' and card_id='%2';").arg(card_type).arg(card_id);
                    S->query->exec(sqls);
                    while(S->query->next()){
                        clientid=S->query->value(0).toString();
                    }
                    int repaymoney=(ui->comboBox_2->currentText().toInt())*365*int((ui->interest_rate->text().toDouble())*(ui->loanmoney->text().toDouble()));
                    attribute.insert(0,clientid);
                    attribute.insert(1,card_id);
                    attribute.insert(2,card_type);
                    attribute.insert(3,QString("%1").arg(maxloan));
                    attribute.insert(4,QString("%1").arg((ui->comboBox_2->currentText().toInt())*365));
                    attribute.insert(5,current_date_time);
                    attribute.insert(6,ui->loanmoney->text());
                    attribute.insert(7,QString("%1").arg(repaymoney+loanmoney));
                    if(!S->AddtoSQL("loan",&attribute)){
                        QMessageBox::information(this, "提示","贷款失败",QMessageBox::Ok);
                        exit(0);
                    };
                    QMessageBox::information(this, "提示","贷款成功",QMessageBox::Ok);
                }
                else {
                    QMessageBox::information(this, "提示","请确认合同书",QMessageBox::Ok);
                }
            }
            else {
                msg="贷款的金额超过你的权限";
                QMessageBox::information(this, "提示",msg,QMessageBox::Cancel);
            }
        }
        else {
            QMessageBox::information(this, "提示","未生成合同",QMessageBox::Cancel);
        }
    }
    else{//存在贷款
        QMessageBox::critical(this, "错误","你还有贷款未还清\n不允许新的贷款",QMessageBox::Cancel);
    }
    ui->hetongshu->clear();
    ui->loanmoney->setText("请输入");
    card_id.clear();
    card_type.clear();
    ui->Screen->setCurrentWidget(ui->screen_page1);
}

void MainWindow::on_zhuanzhang_ok_clicked()
{
    QString zhuanzhangtype=ui->cardtype_zhuanzhang->currentText();
    //判断转帐方卡号是否存在
    QString cardid=ui->cardid_zhuanzhang->text();
    //查询数据库
    QString sqls=QString("SELECT count(*) FROM card_boc where card_id='%1' and card_type='%2';")
            .arg(cardid).arg(zhuanzhangtype);
    int count=-1;
    S->query->exec(sqls);
    while (S->query->next()) {
        count=S->query->value(0).toInt();
    }
    //不存在该卡
    if(count==0){
        QMessageBox::warning(this, "警告","转账对象卡号不存在",QMessageBox::Cancel);
    }
    //存在卡号
    else {
        int truemoney=0;
        int money=ui->money_zhuanzhang->text().toInt();
        //卡类别是否相同
        truemoney = (zhuanzhangtype==card_type) ? (money-int(money*0.02)) : (money-int(money*0.05));
        int overage=0;
        //判断转账方金额是否足够
        sqls=QString("select overage from card_boc where card_id='%1' and card_type='%2';")
                .arg(card_id).arg(card_type);
        S->query->exec(sqls);
        while (S->query->next()) {
            overage=S->query->value(0).toInt();
        }
        //余额不足不允许转账
        if(overage<money){
            QMessageBox::warning(this, "警告","余额不足",QMessageBox::Cancel);
        }
        else{
            //转账方金额减少money,接收方金额增加truemoney
            sqls=QString("update card_boc set overage=%1 where card_type='%2' and card_id='%3';")
                    .arg(overage-money).arg(card_type).arg(card_id);
            S->UpdatefromSQL(sqls);
            sqls=QString("update card_boc set overage=overage+%1 where card_type='%2' and card_id='%3';")
                    .arg(truemoney).arg(zhuanzhangtype).arg(cardid);
            S->UpdatefromSQL(sqls);
            QString msg=QString("转账成功,转账金额为%1元").arg(truemoney);
            QMessageBox::information(this, "提示",msg,QMessageBox::Ok);
        }
    }
    ui->cardid_zhuanzhang->clear();
    ui->money_zhuanzhang->clear();
    card_id.clear();
    card_type.clear();
    ui->Screen->setCurrentWidget(ui->screen_page1);
}

void MainWindow::on_zhuanzhang_clicked()
{
    authdlg.exec();
    pagechange("zhuanzhang");
    login=false;
}

void MainWindow::on_huandai_clicked()
{
    ui->daikuantable->setRowCount(7);     //设置行数为7
    ui->daikuantable->setColumnCount(1);   //设置列数为1
    ui->daikuantable->setColumnWidth(0,500);
    QStringList header;
    header<<"姓名"<<"卡号"<<"卡类别"<<"贷款金额"<<"贷款开始时间"<<"贷款总时间"<<"未还贷款";
    ui->daikuantable->horizontalHeader()->setVisible(false);   //隐藏列表头
    ui->daikuantable->setVerticalHeaderLabels(header);
    authdlg.exec();
    QString sqls=QString("SELECT count(*) FROM loan where card_id='%1' and card_type='%2';").arg(card_id).arg(card_type);
    int count=0;
    S->query->exec(sqls);
    while(S->query->next()){
        count=S->query->value(0).toInt();
    }
    if(count==0&&login==true){
        QMessageBox::information(this, "提示","你没有贷款需要还",QMessageBox::Ok);
        card_id.clear();
        card_type.clear();
        ui->Screen->setCurrentWidget(ui->screen_page1);
        login=false;
    }
    else {
        pagechange("huandai");
        if(login==true){
            QStringList indexitem;
            //查询数据库
            sqls=QString("select name,loan_amount,loan_time,loan_start,repayment from client,loan where client.card_id=loan.card_id and  client.card_type=loan.card_type and client.card_id='%1' and client.card_type='%2';")
                    .arg(card_id).arg(card_type);
            S->query->exec(sqls);
            while(S->query->next()){
                indexitem<<S->query->value(0).toString()<<S->query->value(1).toString()<<S->query->value(2).toString()<<S->query->value(3).toString()<<S->query->value(4).toString();
            }
            indexitem<<card_id<<card_type;
            ui->daikuantable->setItem(0,0,new QTableWidgetItem(indexitem.at(0)));
            ui->daikuantable->setItem(1,0,new QTableWidgetItem(indexitem.at(5)));
            ui->daikuantable->setItem(2,0,new QTableWidgetItem(indexitem.at(6)));
            ui->daikuantable->setItem(3,0,new QTableWidgetItem(indexitem.at(1)));
            ui->daikuantable->setItem(5,0,new QTableWidgetItem(indexitem.at(2)));
            ui->daikuantable->setItem(4,0,new QTableWidgetItem(indexitem.at(3)));
            ui->daikuantable->setItem(6,0,new QTableWidgetItem(indexitem.at(4)));
        }
    }

}

void MainWindow::on_huandai_ok_clicked()
{
    //先只还贷款，不算利息
    int money=ui->money_huandai->text().toInt();
    //int loan_money=ui->daikuantable->item(3,0)->text().toInt();
    int repay_money=ui->daikuantable->item(6,0)->text().toInt();
    //1.查询余额
    int overage=0;
    QString sqls=QString("select overage from card_boc where card_id='%1' and card_type='%2';")
            .arg(card_id).arg(card_type);
    S->query->exec(sqls);
    while(S->query->next()){
        overage=S->query->value(0).toInt();
    }
    //判断卡里余额是否够还贷款
    if(overage>money && repay_money>0 ){//可以
        if(money>=repay_money)money=repay_money;
        overage=overage-money;
        //先更新card_boc
        sqls=QString("update card_boc set overage=%1 where card_type='%2' and card_id='%3';")
                .arg(overage).arg(card_type).arg(card_id);
        S->UpdatefromSQL(sqls);
        //更新贷款表
        //没还完贷款
        if(repay_money>money){
            repay_money=repay_money-money;
            sqls=QString("update loan set repayment=%1 where card_type='%2' and card_id='%3';").arg(repay_money).arg(card_type).arg(card_id);
            S->UpdatefromSQL(sqls);
        }
        else{//还完了贷款，从数据库中删除该信息
            sqls=QString("delete from loan where card_type='%1' and card_id='%2';").arg(card_type).arg(card_id);
            S->DeletefromSQL(sqls);
        }
        QMessageBox::information(this, "提示","还款成功",QMessageBox::Ok);
    }
    else {
        QMessageBox::warning(this, "提示","余额不足",QMessageBox::Cancel);
    }
    card_id.clear();
    card_type.clear();
    ui->Screen->setCurrentWidget(ui->screen_page1);
    login=false;
}
void MainWindow::PushButtonClick(){
    int index=Group->checkedId();
    QString sqls="";
    QString money="";
    switch (index) {
    case 0:
    {//存款金额
        ui->tableWidget_3->setItem(0,3,new QTableWidgetItem("存款"));
        sqls=QString("select overage from card_boc where card_id='%1' and card_type='%2';").arg(card_id).arg(card_type);
        S->query->exec(sqls);
        while(S->query->next()){
            money=S->query->value(0).toString();
        }
        ui->tableWidget_3->setItem(0,4,new QTableWidgetItem(money));
        break;
    }
    case 1:
    {//存款金额
        ui->tableWidget_3->setItem(0,3,new QTableWidgetItem("贷款"));
        sqls=QString("select repayment from loan where card_id='%1' and card_type='%2';").arg(card_id).arg(card_type);
        S->query->exec(sqls);
        while(S->query->next()){
            money=S->query->value(0).toString();
        }
        ui->tableWidget_3->setItem(0,4,new QTableWidgetItem(money));
        break;
    }
    default:
        break;
    }
}

void MainWindow::on_yu_e_clicked()
{
    authdlg.exec();
    pagechange("yu_e");
    if(login==true){
        QString name="";
        QString sqls=QString("select name from client where card_id='%1' and card_type='%2';")
                .arg(card_id).arg(card_type);
        S->query->exec(sqls);
        while(S->query->next()){
            name=S->query->value(0).toString();
        }

        ui->tableWidget_3->setItem(0,0,new QTableWidgetItem(name));
        ui->tableWidget_3->setItem(0,1,new QTableWidgetItem(card_type));
        ui->tableWidget_3->setItem(0,2,new QTableWidgetItem(card_id));
    }
    login=false;
}

void MainWindow::on_Button_kaihu_clicked()
{
    opendlg.exec();
}

void MainWindow::on_Button_xiaohu_clicked()
{
    deletedlg.exec();
}

void MainWindow::on_admin_query_clicked()
{
    //    //组合查询,贷款
    //    //选择非空的值
    QString name=ui->admin_name->text();
    QString cardid=ui->admin_id->text();
    int days=ui->admin_combox->currentText().toInt()*365;
    //    QMap<QString,QString> map;
    //    if(name!="")map["name"]=name;
    //    if(cardid!="")map["cardid"]=cardid;
    //    QString sqlsname=QString("select overage from client,card_boc where card_boc.card_id=client.card_id and card_boc.card_type=client.card_type and name='%1';").arg(name);
    //    QString sqlscardid=QString("select overage from card_boc where card_id='%1'").arg(cardid);
    //    QString Sqlsidname=QString("select overage from client,card_boc where card_boc.card_id=client.card_id and card_boc.card_type=client.card_type and name='%1' and client.card_id='%2';")
    //            .arg(name).arg(cardid);
    if(days>0){//查询贷款
        ui->admin_table->clear();
        QString sqls=QString("select client.name,loan.client_id,loan.card_id,loan.card_type,loan.loan_high_amount,loan.loan_time,loan.loan_start,loan.loan_amount,loan.repayment from client,loan where client.card_id=loan.card_id and  client.card_type=loan.card_type and (loan.loan_time=%1 or client.card_id='%2' or client.name='%3');")
                .arg(days).arg(cardid).arg(name);
        S->query->exec(sqls);
        QStringList Ops;
        while(S->query->next()){
            for(int i=0;i<9;i++){
                Ops<<S->query->value(i).toString();
            }
        }
        int lenth=Ops.length()/9;
        ui->admin_table->setRowCount(lenth);
        ui->admin_table->setColumnCount(9);   //设置列数为1
        ui->admin_table->setColumnWidth(0,80);
        QStringList header;
        header<<"姓名"<<"客户ID"<<"卡号"<<"卡类别"<<"最高贷款金额"<<"贷款总时间"<<"贷款开始时间"<<"贷款总金额"<<"未还贷款";
        ui->admin_table->verticalHeader()->setVisible(false);   //隐藏列表头
        ui->admin_table->setHorizontalHeaderLabels(header);
        for(int i=0;i<lenth;i++){
            for(int j=0;j<9;j++){
                ui->admin_table->setItem(i,j,new QTableWidgetItem(Ops.at(9*i+j)));
            }
        }
    }
    //模糊查询，存款
    else {
        ui->admin_table->clear();
        QString sqls=QString("select overage,client.name from client,card_boc where card_boc.card_id=client.card_id and card_boc.card_type=client.card_type and name LIKE '%%1%';").arg(name);
        S->query->exec(sqls);
        QStringList Ops;
        while(S->query->next()){
            for(int i=0;i<2;i++){
                Ops<<S->query->value(i).toString();
            }
        }
        int lenth=Ops.length()/2;
        ui->admin_table->setRowCount(lenth);
        ui->admin_table->setColumnCount(1);   //设置列数为1
        ui->admin_table->setColumnWidth(0,800);
        QStringList header;
        header<<"存款";
        ui->admin_table->setHorizontalHeaderLabels(header);
        header.clear();
        for(int i=0;i<lenth;i++){
            header<<Ops.at(2*i+1);
            ui->admin_table->setItem(i,0,new QTableWidgetItem(Ops.at(2*i)));

        }
        ui->admin_table->setVerticalHeaderLabels(header);
    }

}

#include"banksql.h"

Banksql::Banksql(){
    db = QSqlDatabase::addDatabase("QODBC");   //数据库驱动类型为SQL Server
    qDebug()<<"ODBC driver?"<<db.isValid();
    QString dsn = QString::fromLocal8Bit("QTDSN");      //数据源名称
    db.setHostName("localhost");                        //选择本地主机，127.0.1.1
    db.setDatabaseName("test");                            //设置数据源名称
    db.setUserName("sa");                               //登录用户
    db.setPassword("123");                              //密码
    if(!db.open())                                      //打开数据库
    {
        qDebug()<<db.lastError().text();
        QMessageBox::critical(nullptr,QObject::tr("Database error"), db.lastError().text());
        exit(0);//打开失败
    }
    else {
        qDebug()<<"database open Succeed!";
        query=new QSqlQuery(db);
    }
}

Banksql::~Banksql(){
    db.close();
    delete query;
}
//在数据库中添加字段
bool Banksql:: AddtoSQL(QString tablename,QList<QString> *attribute){
    QMap<QString,int> namemap;
    namemap["client"]=0;
    namemap["card_boc"]=1;
    namemap["loan"]=2;
    namemap["overdraft"]=3;
    namemap["authenticate"]=4;
    namemap["admin"]=5;
    int x=namemap[tablename];
    switch (x) {
    case 0:
    {
        QString sqls = QString("insert into client values('%1','%2','%3','%4',%5);")
                .arg(attribute->at(0)).arg(attribute->at(1)).arg(attribute->at(2)).arg(attribute->at(3)).arg(attribute->at(4).toInt());
        qDebug()<<sqls;
        bool re=query->exec(sqls);

        if(!re){
            qDebug()<<"fail to add new data to client";
            return false;
        }
        else {
            qDebug()<<"succeed to add new data to client";
        }
    }
        break;
    case 1:
    {
        QString sqls = QString("insert into card_boc values('%1','%2',%3,%4,%5,%6);")
                .arg(attribute->at(0))
                .arg(attribute->at(1))
                .arg(attribute->at(2).toInt())
                .arg(attribute->at(3).toInt())
                .arg(attribute->at(4).toInt())
                .arg(attribute->at(5).toInt());
        bool re=query->exec(sqls);
        if(!re){
            qDebug()<<"fail to add new data to card_boc";
            return false;
        }
        else {
            qDebug()<<"succeed to add new data to card_boc";
        }
    }
        break;
    case 2:
    {
        QString sqls = QString("insert into loan values('%1','%2','%3',%4,%5,%6,%7,%8);")
                .arg(attribute->at(0)).arg(attribute->at(1)).arg(attribute->at(2))
                .arg(attribute->at(3).toInt()).arg(attribute->at(4).toInt())
                .arg(QString("convert(varchar(30),'%1',23)").arg(attribute->at(5)))
                .arg(attribute->at(6).toInt())
                .arg(attribute->at(7).toInt());
        qDebug()<<sqls;
        bool re=query->exec(sqls);
        if(!re){
            qDebug()<<"fail to add new data to loan";
            qDebug()<<query->lastError();
            return false;
        }
        else {
            qDebug()<<"succeed to add new data to loan";
        }
    }
        break;
    case 3:
    {
        QString sqls = QString("insert into overdraft values('%1','%2','%3',%4,%5,%6);")
                .arg(attribute->at(0)).arg(attribute->at(1)).arg(attribute->at(2))
                .arg(attribute->at(3).toInt())
                .arg(QString("convert(varchar(30),'%1',23)").arg(attribute->at(4)))
                .arg(attribute->at(5).toInt());
        qDebug()<<sqls;
        bool re=query->exec(sqls);
        if(!re){
            qDebug()<<QString("fail to add new data to %1").arg(tablename);
            return false;
        }
        else {
            qDebug()<<QString("succeed to add new data to %1").arg(tablename);
        }
    }
        break;
    case 4:
    {
        QString sqls = QString("insert into authenticate values('%1','%2','%3');")
                .arg(attribute->at(0)).arg(attribute->at(1)).arg(attribute->at(2));
        bool re=query->exec(sqls);
        if(!re){
            qDebug()<<QString("fail to add new data to %1").arg(tablename);
            return false;
        }
        else {
            qDebug()<<QString("succeed to add new data to %1").arg(tablename);
        }
    }
        break;
    case 5:
    {
        QString sqls = QString("insert into admin values('%1','%2');")
                .arg(attribute->at(0)).arg(attribute->at(1));
        bool re=query->exec(sqls);
        if(!re){
            qDebug()<<QString("fail to add new data to %1").arg(tablename);
            return false;
        }
        else {
            qDebug()<<QString("succeed to add new data to %1").arg(tablename);
        }
    }
        break;
    default:
        qDebug()<<"Add query name failed!";
        return false;
    }
    return true;
}
//删除--条件删除
bool Banksql::DeletefromSQL(QString sqls){
    bool re=query->exec(sqls);
    if(!re){
        qDebug()<<"failed to delete";
        qDebug()<<query->lastError();
        return false;
    }
    else {
        qDebug()<<"succeed to delete";

    }
    return true;
}
//更新--条件更新
bool Banksql::UpdatefromSQL(QString sqls){
    bool re=query->exec(sqls);
    if(!re){
        qDebug()<<"failed to update";
        qDebug()<<query->lastError();
        return false;
    }
    else {
        qDebug()<<"succeed to update";
    }
    return true;
}

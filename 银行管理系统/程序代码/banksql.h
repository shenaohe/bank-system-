#ifndef BANKSQL_H
#define BANKSQL_H
#include<QSql>
#include<QDialog>
#include<QDebug>
#include<QMessageBox>
#include<QSqlError>
#include<QSqlDatabase>
#include<QSqlQuery>
#include"QList"
class Banksql{
public:
    //初始化数据库，连接
    Banksql();
    //断开连接
    ~Banksql();
    //增
    bool AddtoSQL(QString tablename,QList<QString> *attribute);//tablename--表名,attribute--属性名
    //删--
    bool DeletefromSQL(QString sqls);//sqls--sql语句，在具体功能中构建
    //查--
    //具有返回值在具体功能中构建
    //改
    bool UpdatefromSQL(QString sqls);//sqls--sql语句，在具体功能中构建
    QSqlQuery* query;
private:
    QSqlDatabase db;//QT数据库API


};
#endif // BANKSQL_H

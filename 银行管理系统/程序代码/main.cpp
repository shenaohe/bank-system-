#include "mainwindow.h"
#include <QApplication>
//#include "banksql.h"
//#include"global_var.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //certificate ce;
    //ce.show();
    w.show();
//    QList<QString> L;
//    //varchar--要加单引号，int--不用加单引号
//    L<<"'007'"<<"'20191003369'"<<"'牡丹卡'"<<"100000"<<"30"<<"'2022-05-02'"<<"10000"<<"9000";
//    if(!S->AddtoSQL("loan",&L)){
//        exit(0);
//    }
//    QString sqls="delete from overdraft where client_id='002';";
//    if(!S->DeletefromSQL(sqls)){
//        exit(0);
//    }
//    QString sqls="update admin set id='001' where id='000';";
//    if(!S->UpdatefromSQL(sqls)){
//        exit(0);
//    }

    return a.exec();
}

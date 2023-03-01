#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "authentication.h"
#include<QButtonGroup>
#include"openaccount.h"
#include"deleteaccount.h"
#include"admindlg.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_clicked()__attribute__((noreturn));

    void on_cunkuan_clicked();

    void on_cunkuan_ok_clicked();

    void on_qukuan_clicked();

    void pagechange(QString pagename);
    void on_qukuan_ok_clicked();

    void on_daikuan_clicked();

    void on_daikuan_ok_clicked();

    void on_comboBox_2_currentIndexChanged(int index);

    void on_daikuan_start_clicked();

    void on_zhuanzhang_ok_clicked();

    void on_zhuanzhang_clicked();

    void on_huandai_clicked();

    void on_huandai_ok_clicked();

    void  PushButtonClick();

    void on_yu_e_clicked();

    void on_Button_kaihu_clicked();

    void on_Button_xiaohu_clicked();

    void on_admin_query_clicked();

private:
    Ui::MainWindow *ui;
    authentication authdlg;
    QButtonGroup *Group;
    openaccount opendlg;
    deleteaccount deletedlg;
    admindlg adminn;
};

#endif // MAINWINDOW_H

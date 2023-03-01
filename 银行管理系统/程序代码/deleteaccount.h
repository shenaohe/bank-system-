#ifndef DELETEACCOUNT_H
#define DELETEACCOUNT_H

#include <QDialog>

namespace Ui {
class deleteaccount;
}

class deleteaccount : public QDialog
{
    Q_OBJECT

public:
    explicit deleteaccount(QWidget *parent = nullptr);
    ~deleteaccount();

private slots:
    void on_xiaohu_clicked();

    void on_find_clicked();

    void on_qukuan_clicked();

    void on_quxiao_clicked();

private:
    Ui::deleteaccount *ui;
};

#endif // DELETEACCOUNT_H

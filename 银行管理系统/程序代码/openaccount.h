#ifndef OPENACCOUNT_H
#define OPENACCOUNT_H

#include <QDialog>

namespace Ui {
class openaccount;
}

class openaccount : public QDialog
{
    Q_OBJECT

public:
    explicit openaccount(QWidget *parent = nullptr);
    ~openaccount();

private slots:
    void on_kaihu_clicked();

    void on_quxiao_clicked();

private:
    Ui::openaccount *ui;
};

#endif // OPENACCOUNT_H

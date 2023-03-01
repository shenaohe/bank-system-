#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <QDialog>
#include"global_var.h"
namespace Ui {
class authentication;
}

class authentication : public QDialog
{
    Q_OBJECT

public:
    explicit authentication(QWidget *parent = nullptr);
    ~authentication();
//signals:
//    void loginsuccess();
private slots:
    void on_exitButton_clicked();

    void on_okButton_clicked();

private:
    Ui::authentication *ui;
};

#endif // AUTHENTICATION_H

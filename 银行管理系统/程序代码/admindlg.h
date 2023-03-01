#ifndef ADMINDLG_H
#define ADMINDLG_H

#include <QDialog>

namespace Ui {
class admindlg;
}

class admindlg : public QDialog
{
    Q_OBJECT

public:
    explicit admindlg(QWidget *parent = nullptr);
    ~admindlg();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::admindlg *ui;
};

#endif // ADMINDLG_H

#ifndef TESTSRESULTS_H
#define TESTSRESULTS_H

#include <QWidget>

namespace Ui {
class testsresults;
}

class testsresults : public QWidget
{
    Q_OBJECT

public:
    explicit testsresults(QWidget *parent = nullptr);
    ~testsresults();

private slots:
    void on_pushButton_clicked();

private:
    Ui::testsresults *ui;
};

#endif // TESTSRESULTS_H

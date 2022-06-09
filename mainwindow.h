#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QtSql\QtSql>
#include "testsresults.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{

    Q_OBJECT
private slots:
    void on_pushButtonExit();
    void on_pushButtonWritetootherCOMport();
    void connecttoSerialPort();
    void nextpageforresult();
    void clsfun();
public:
    void setText(QString testrequest);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort*serial=nullptr;
    QMessageBox*msgBox;
    testsresults *Twindow;
};
#endif // MAINWINDOW_H

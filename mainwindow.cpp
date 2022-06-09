#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "testsresults.h"
#include "constants.h"
#include "windows.h"
#include "seria2.h"
#include "serial.h"
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextCodec>
#include <QApplication>
#include <QFile>
#include <QFileInfo>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    try {
        connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(on_pushButtonWritetootherCOMport()));
        connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(on_pushButtonExit()));
        connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(connecttoSerialPort()));
        connect(ui->clsbut,SIGNAL(clicked()),this,SLOT(clsfun()));
        connect(ui->checktests_btn,SIGNAL(clicked()),this,SLOT(nextpageforresult()));
        const auto infos = serial::get().getlistnameport();
        for (const auto &info : infos) {
            ui->PortComboBox->addItem(info);
        }
    }
    catch (const std::bad_alloc &) {
        qDebug()<<"Exeception";
    }
}
MainWindow::~MainWindow()
{
    delete ui;
    serial->close();
}
void MainWindow::setText(QString testrequest) {
    ui->textEdit->append(testrequest);
}
void MainWindow::clsfun() {
    ui->textEdit->setText("");
}
void MainWindow::connecttoSerialPort() {
    if (ui->PortComboBox->currentText()!="") {
       serial::get().setportnumber(ui->PortComboBox->currentText());
    } else QMessageBox::critical(this, tr("error"), "Вы не выбрали порт");
}

void MainWindow::nextpageforresult() {
    Twindow = new testsresults(this);
    Twindow->show();
}
//24 01 26434333 65 2D 44235511 56764565345466457643321232 10813178
void MainWindow::on_pushButtonExit()
{
    msgBox->setText("Выход");
}
void MainWindow::on_pushButtonWritetootherCOMport()
{
    QString gettext = ui->textEdit->toPlainText().trimmed();
    QByteArray ba = gettext.toUtf8();
    serial->write(ba);
}

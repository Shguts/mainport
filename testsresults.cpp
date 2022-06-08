#include "testsresults.h"
#include "ui_testsresults.h"
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextCodec>
#include <QApplication>
#include <QFile>
#include <QFileInfo>

testsresults::testsresults(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::testsresults)
{
    ui->setupUi(this);
    QFile file(".//results.txt");
    if (!file.open(QFile::ReadOnly|QFile::Text)) {
        qDebug()<<"Файл не открылся";
        return;
    }
    QTextStream stream(&file);
    QString get_txt_results = stream.readAll();
    ui->get_file_txt->setText(get_txt_results);
    file.flush();
    file.close();
}

testsresults::~testsresults()
{
    delete ui;
}

void testsresults::on_pushButton_clicked()
{
    close();
}


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
    void serialRecieve();
    void handleError(QSerialPort::SerialPortError error);
    void on_pushButtonExit();
    void on_pushButtonWritetootherCOMport();
    void connecttoSerialPort();
    void nextpageforresult();
    void clsfun();

public slots:
    void initiat() {
        queue.buffer = (uint8_t*)malloc(19);
        queue.buffer_size = 19;
        queue.head = 0;
        queue.tail = 0;
        queue.bytes_avail=0;
    }
public:
    MainWindow(QWidget *parent = nullptr);
    typedef struct {
        uint8_t*buffer;
        size_t buffer_size;
        size_t head;
        size_t tail;
        size_t bytes_avail;

    } queue_t;
    struct HEADing {
        uint8_t Separatorattribute = 0;
        uint8_t Namecomand;
        uint16_t LengthMessage;
        uint8_t Indifier;
        uint8_t ControSum ;
    };
    struct BODYmsg {
        uint32_t TS1;
        QList<uint8_t> TS2;
        uint32_t TS3;
    };
    int genid;
    queue_t queue;
    BODYmsg databodymess;
    HEADing datahead;
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort*serial=nullptr;
    QMessageBox*msgBox;
    testsresults *Twindow;
};
#endif // MAINWINDOW_H

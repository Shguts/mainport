#ifndef SERIAL_H
#define SERIAL_H
#include "singleton.h"
#include "utils.h"
#include "constants.h"
#include "seria2.h"
#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QObject>
#include <QSharedPointer>

enum class namecommands{
    readyforDevice = 0x01,
    writeSPO = 0x02,
    writeSI = 0x04,
    clearSPO = 0x03,
    writeSINum = 0x05,
    clearSI = 0x06,
    clearSINum = 0x07,
    clearSelectSiNum = 0x08,
    writeGSC = 0x10
};

template<class invoke_enum = namecommands>
class execution {
public:
    execution() {};
    template<typename ret_type, typename base_type, typename...args_type>
    void push_event(invoke_enum invoke_key,ret_type&result,base_type& base,ret_type(base_type::*fn)(args_type...),args_type&&...args) {
        this->execommand.insert({invoke_key, [&base,&result,fn, &args...] (){
                                 result = (base.*fn)(args...);
                                 }});
    }
    auto operator()(invoke_enum i) {
        return execommand[i];
    }
private:
    std::map<invoke_enum, std::function<void(void)>> execommand;
};

class serial:public QObject,public Singleton<serial>
{
    Q_OBJECT
private slots:
    void handleError(QSerialPort::SerialPortError error);
    void serialRecieve();
    friend class Singleton<serial>;
private:
    unsigned char SPOmassive[3] = {0x33,0x28,0x07};
    QByteArray TOWRITE_FOR_SPO{};
    QByteArray data;
    uint8_t shufflearr[80] = {};
    uint8_t GSCarray[160] = {};
    utilities::types::ringBuffer::queue_t queue;
    utilities::types::BODYmsg datarecord;
    utilities::types::HEADing headrecord;
    MainWindow*ui;
    QByteArray ret;
    int size = 160;
    int getsuzesoftypes = 6;
    execution<namecommands>commandsex;
    QSharedPointer<QSerialPort> m_serial;
    QList<QSerialPortInfo> m_info;
    void writetofiles(QByteArray ba);
    QByteArray functionforwrite(uint32_t operationfalse,QByteArray data);
    QByteArray writeSINum(QByteArray&data);
    QByteArray writeSI(QByteArray&data);
    QByteArray clearSelectSiNum(QByteArray&data);
    QByteArray clearSINum(QByteArray&data);
    QByteArray writeforGSC(QByteArray&data);
    QByteArray readyforDevice(QByteArray&data);
    QByteArray writeSPO(QByteArray&data);
    void writetoTS1(utilities::types::ringBuffer::queue_t *q,utilities::types::BODYmsg*datarecord);
    void writetoTS2(utilities::types::ringBuffer::queue_t *q,utilities::types::BODYmsg*datarecord,utilities::types::HEADing*headrecord);
    void writetoTS3(utilities::types::ringBuffer::queue_t *q,utilities::types::BODYmsg*datarecord);
    QByteArray writerostruct();
    void optionalsforport() {
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);
        bool result = m_serial->open(QIODevice::ReadWrite);
        if (!result) {
            qDebug()<<"Failed to open port:"<< m_serial->portName()<<"error"<<m_serial->errorString();
        } else QMessageBox(QMessageBox::Icon::Information,"ok", "Вы подключились к порту",QMessageBox::Yes).exec();
    }
public:
    void getMainWindow(MainWindow*ui)
    {

        this->ui = ui;
    }
    void showdown() {
        m_serial->close();
    }
    void initialize() {
        commandsex.push_event(namecommands::readyforDevice,ret,*this,&serial::readyforDevice,data);
        commandsex.push_event(namecommands::writeSPO,ret,*this,&serial::writeSPO,data);
        commandsex.push_event(namecommands::writeGSC,ret,*this,&serial::writeforGSC,data);
        commandsex.push_event(namecommands::writeSI,ret,*this,&serial::writeSI,data);
        commandsex.push_event(namecommands::writeSINum,ret,*this,&serial::writeSINum,data);
        commandsex.push_event(namecommands::clearSelectSiNum,ret,*this,&serial::clearSelectSiNum,data);
        commandsex.push_event(namecommands::clearSINum,ret,*this,&serial::clearSINum,data);
        m_serial.reset(new QSerialPort());
        ui = (MainWindow*)QApplication::activeWindow();
        m_info = QSerialPortInfo::availablePorts();
        connect(m_serial.get(),&QSerialPort::errorOccurred,this,&serial::handleError);
        connect(m_serial.get(),&QSerialPort::readyRead,this,&serial::serialRecieve);
        queue.buffer = (uint8_t*)malloc(19);
        queue.buffer_size = 19;
        queue.head = 0;
        queue.tail = 0;
        queue.bytes_avail=0;
        data = QByteArray(174,'\0');
        ret = QByteArray(174,'\0');

    }
    void setportnumber(const QString& portNumber) {
        m_serial->setPortName(portNumber);
        optionalsforport();
    }
    QList<QString> getlistnameport() {
        QList<QString> list_ports;
        for (const QSerialPortInfo &info : m_info) {
            list_ports.append(info.portName());
        }
        return list_ports;

    }
    serial() {
    };
};

#endif // SERIAL_H

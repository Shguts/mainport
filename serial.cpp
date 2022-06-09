#include "serial.h"
#include <QFile>


void serial::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) QMessageBox(QMessageBox::Icon::Critical, "Critical Error", m_serial->errorString()).exec();
}
void serial::writetofiles(QByteArray ba) {
    QFile file(".//results.txt");
    if (!file.open(QFile::WriteOnly|QFile::Text)) {
        qDebug()<<"Файл не открылся";
        return;
    }
    QTextStream stream(&file);
    stream<<ba;
    file.close();
}
void serial::serialRecieve()
{
    QByteArray ba;
    QString fropdata;
    auto g = m_serial->bytesAvailable();
    auto x = m_serial->readAll();
    fropdata = utilities::ringBuffer::put(&queue,x,g);
    qint32 i = 0;
    int id=0;
    // uint32_t operationfalse = 0x80000000;
    while (i<fropdata.size()-id) {
        if (i%2==0&&i!=0)
        {
            fropdata.insert(i+id,'-');
            id++;
        }
        i++;
    }
    while (queue.bytes_avail) {
        if (headrecord.Separatorattribute==0x24) {
            if ((queue.tail-queue.head>=headrecord.LengthMessage&&queue.tail>queue.head)||(queue.head>queue.tail&&queue.buffer_size-queue.head+queue.tail>=headrecord.LengthMessage)) {
                m_serial->write(writerostruct());
            }
            else return;
        }
        if (*(queue.buffer+queue.head)==0x24&&queue.bytes_avail>=6) {
            if(utilities::CRC::crc8ccitt(0,queue.buffer+queue.head,5,&queue)==*(queue.buffer+(queue.head+5)%queue.buffer_size)) {
                qDebug()<<"ok";
                headrecord.Separatorattribute = utilities::ringBuffer::get(&queue,sizeof(headrecord.Separatorattribute));
               headrecord.Namecomand=utilities::ringBuffer::get(&queue,sizeof(headrecord.Namecomand));
                headrecord.LengthMessage=utilities::ringBuffer::get(&queue,sizeof(headrecord.LengthMessage));
                headrecord.Indifier=utilities::ringBuffer::get(&queue,sizeof(headrecord.Indifier));
                headrecord.ControSum = utilities::ringBuffer::get(&queue,sizeof(headrecord.ControSum));
                if ((queue.tail-queue.head>=headrecord.LengthMessage&&queue.tail>=queue.head)||(queue.head>queue.tail&&queue.buffer_size-queue.head+queue.tail>=headrecord.LengthMessage)) {
                    QByteArray getrequest = writerostruct();
                    m_serial->write(getrequest);
                    writetofiles(getrequest);
                }
                else {
                    ui->setText(fropdata);
                    return;
                }
            }
        } else {
            if(queue.bytes_avail){
                queue.head+=1%queue.buffer_size;
                queue.bytes_avail--;
            }
        }
    }
    ui->setText(fropdata);
    QMessageBox(QMessageBox::Icon::Information,"ok", "Чтение завершено",QMessageBox::Yes).exec();
}
void serial::writetoTS1(utilities::types::ringBuffer::queue_t *q,utilities::types::BODYmsg*datarecord) {
    datarecord->TS1 = utilities::ringBuffer::get(q,sizeof(datarecord->TS1));
}
void serial::writetoTS2(utilities::types::ringBuffer::queue_t *q,utilities::types::BODYmsg*datarecord,utilities::types::HEADing*headrecord) {
    for (int i=0;headrecord->LengthMessage-8>i;i++) {
        if (q->head==q->buffer_size) {
            q->head = q->head%q->buffer_size;
            datarecord->TS2.append(*(q->buffer+(q->head++)));
            q->bytes_avail--;
        }
        else {
            datarecord->TS2.append(*(q->buffer+(q->head++)));
            q->bytes_avail--;
        }
    }
    q->head%=q->buffer_size;
}
void serial::writetoTS3(utilities::types::ringBuffer::queue_t *q,utilities::types::BODYmsg*datarecord) {
    datarecord->TS3 = utilities::ringBuffer::get(q,sizeof(datarecord->TS3));
}
QByteArray serial::functionforwrite(uint32_t operationfalse,QByteArray data) {
    memcpy(data.data()+getsuzesoftypes,&(operationfalse),4);
    getsuzesoftypes+=sizeof(datarecord.TS1);
    uint32_t getnoewCSTS = utilities::CRC::crc32i(0, (data.data()), 4);
    memcpy(data.data()+getsuzesoftypes,&(getnoewCSTS),4);
    data.resize(16);
    return data;

}
/////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
QByteArray serial::writeSPO(QByteArray&data){
    uint8_t*headformember = queue.buffer+queue.head;
    if (utilities::CRC::crc32i(0,headformember,headrecord.LengthMessage-4,&queue)==(utilities::ringBuffer::get(&queue,&headrecord))) {
        writetoTS1(&queue,&datarecord);
        uint32_t getreevrsedig=0;
        uint8_t massiveforTS1[4];
        for (int i=0;i<sizeof(massiveforTS1);i++) {
            massiveforTS1[i]=datarecord.TS1>>(8*(3-i));
            getreevrsedig += massiveforTS1[i]<<(8*i);
        }
        datarecord.TS1 = getreevrsedig;
        datarecord.TS2.clear();
        writetoTS2(&queue,&datarecord,&headrecord);
        writetoTS3(&queue,&datarecord);
        for (int i=0;i<datarecord.TS2.size();i++) {
            TOWRITE_FOR_SPO.append(datarecord.TS2[i]);
        }
        if (TOWRITE_FOR_SPO.size()==datarecord.TS1) {
            return functionforwrite(UVK20_RES_OK_SPOLOAD,data);
        }
        else if (TOWRITE_FOR_SPO.size()<datarecord.TS1){
            return functionforwrite(UVK20_RES_OK,data);
        }
        else if (datarecord.TS2.size()>datarecord.TS1) {
            return functionforwrite(UVK20_RES_FAIL_SPOLOAD,data);
        }

    } else {
        return functionforwrite(UVK20_RES_FAIL_CS,data);
    }
}

QByteArray serial::readyforDevice(QByteArray&data){
    uint32_t operationfalse = UVK20_RES_OK_SIERASE;
    if (headrecord.Namecomand==0x06) {
        if (seria2[0]==0xff&&seria2[1]==0x44) memset(seria2,0,1280*sizeof (uint8_t)); else operationfalse = UVK20_RES_FAIL_SIERASE;
    }
    if (headrecord.Namecomand==0x03) {
        if(SPOmassive[0]==0x33) memset(SPOmassive,0,1280*sizeof (unsigned char)); else operationfalse = UVK20_RES_OK;
    }
    memcpy(data.data()+getsuzesoftypes,&(operationfalse),4);
    uint32_t getnoewCSTS = utilities::CRC::crc32i(0, (data.data()+getsuzesoftypes), 4);
    getsuzesoftypes+=sizeof (datarecord.TS1);
    memcpy(data.data()+getsuzesoftypes,&(getnoewCSTS),4);
    data.resize(16);
    return data;
}
QByteArray serial::writeforGSC(QByteArray&data){
    uint32_t operationfalse = UVK20_RES_OK_SIERASE;
    for (int i=0;i<size;i++) {
        GSCarray[i] = rand();
    }
    memcpy(data.data()+getsuzesoftypes,&(operationfalse),4);
    getsuzesoftypes+=sizeof (datarecord.TS1);
    for (int i=0;i<size;getsuzesoftypes++,i++)
        memcpy(data.data()+getsuzesoftypes,&GSCarray[i],1);
    uint32_t getnoewCSTS = utilities::CRC::crc32i(0, (data.data()+8), size+4);
    memcpy(data.data()+getsuzesoftypes,&(getnoewCSTS),4);
    data.resize(size+14);
    return data;
}

QByteArray serial::clearSINum(QByteArray&data){
    uint8_t*headformember = queue.buffer+queue.head;
    uint32_t operationfalse;
    if (utilities::CRC::crc32i(0,headformember,headrecord.LengthMessage-4,&queue)==*(uint32_t*)((queue.buffer+(queue.head+headrecord.LengthMessage-4)%queue.buffer_size))) {
        operationfalse = UVK20_RES_OK_SIERASE;
        if (seria2[7]==datarecord.TS1) {
            memset(seria2,0,1280*sizeof (uint8_t));
        } else {
            if (seria2[1279]==datarecord.TS1) {
                memset(seria2,1280,1280*sizeof (uint8_t));
            }
        }
    }
    else {
        operationfalse = UVK20_RES_FAIL_CS;
    }
    memcpy(data.data()+getsuzesoftypes,&(operationfalse),4);
    uint32_t getnoewCSTS = utilities::CRC::crc32i(0, (data.data()+getsuzesoftypes), 4);
    getsuzesoftypes+=sizeof (datarecord.TS1);
    memcpy(data.data()+getsuzesoftypes,&(getnoewCSTS),4);
    data.resize(16);
    return data;
}

QByteArray serial::clearSelectSiNum(QByteArray&data){
    uint8_t*headformember = queue.buffer+queue.head;
    uint32_t operationfalse;
    if (utilities::CRC::crc32i(0,headformember,headrecord.LengthMessage-4,&queue)==*(uint32_t*)((queue.buffer+(queue.head+headrecord.LengthMessage-4)%queue.buffer_size))) {
        writetoTS1(&queue,&datarecord);
        writetoTS2(&queue,&datarecord,&headrecord);
        writetoTS3(&queue,&datarecord);
        operationfalse = UVK20_RES_OK;
    }
    else {
        operationfalse = UVK20_RES_FAIL_CS;
    }
    //проверка на запись и стирание в зависимости от команды
    memcpy(data.data()+getsuzesoftypes,&(operationfalse),4);
    uint32_t getnoewCSTS = utilities::CRC::crc32i(0, (data.data()+getsuzesoftypes), 4);
    getsuzesoftypes+=sizeof (datarecord.TS1);
    memcpy(data.data()+getsuzesoftypes,&(getnoewCSTS),4);
    data.resize(16);
    return data;
}

QByteArray serial::writeSI(QByteArray&data){
    uint8_t*headformember = queue.buffer+queue.head;
    uint8_t keyarr[80] = {};
    uint32_t operationfalse;
    if (utilities::CRC::crc32i(0,headformember,headrecord.LengthMessage-4,&queue)==*(uint32_t*)((queue.buffer+(queue.head+headrecord.LengthMessage-4)%queue.buffer_size))) {
        writetoTS1(&queue,&datarecord);
        writetoTS2(&queue,&datarecord,&headrecord);
        writetoTS3(&queue,&datarecord);
        operationfalse = UVK20_RES_OK;
        for (int i=0;i<size/2;i++) {
            datarecord.TS2[i]^=GSCarray[i];
            keyarr[shufflearr[i]] = datarecord.TS2[i];
        }
        if (keyarr[3]!=seria2[3+80*keyarr[3]]) {
            memcpy(&seria2[80*keyarr[3]],keyarr,sizeof (keyarr));
        } else {
            operationfalse = UVK20_RES_FAIL_CMND;
        }
    }
    else {
        operationfalse = UVK20_RES_FAIL_CS;
    }
    memcpy(data.data()+getsuzesoftypes,&(operationfalse),4);
    uint32_t getnoewCSTS = utilities::CRC::crc32i(0, (data.data()+getsuzesoftypes), 4);
    getsuzesoftypes+=sizeof (datarecord.TS1);
    memcpy(data.data()+getsuzesoftypes,&(getnoewCSTS),4);
    data.resize(16);
    return data;
}

QByteArray serial::writeSINum(QByteArray&data){
    uint8_t*headformember = queue.buffer+queue.head;
    uint32_t operationfalse;
    uint8_t keyarr[80] = {};
    for (int i=0;i<size/2;i++) {
        shufflearr[i]=i;
    }
    for (int i=0,indexforchange=0,bubble=0;i<size/2;i++) {
        indexforchange = GSCarray[i+80]%80;
        bubble = shufflearr[indexforchange];
        shufflearr[indexforchange] = shufflearr[i];
        shufflearr[i] = bubble;
    }
    if (utilities::CRC::crc32i(0,headformember,headrecord.LengthMessage-4,&queue)==*(uint32_t*)((queue.buffer+(queue.head+headrecord.LengthMessage-4)%queue.buffer_size))) {
        writetoTS1(&queue,&datarecord);
        writetoTS2(&queue,&datarecord,&headrecord);
        writetoTS3(&queue,&datarecord);
        operationfalse = UVK20_RES_OK;
        for (int i=0;i<size/2;i++) {
            datarecord.TS2[i]^=GSCarray[i];
            keyarr[shufflearr[i]] = datarecord.TS2[i];
        }
        if (keyarr[3]!=seria2[3+80*keyarr[3]]) {
            memcpy(&seria2[80*keyarr[3]],keyarr,sizeof (keyarr));
        } else {
            operationfalse = UVK20_RES_FAIL_CMND;
        }
    }
    else {
        operationfalse = UVK20_RES_FAIL_CS;
    }
    memcpy(data.data()+getsuzesoftypes,&(operationfalse),4);
    uint32_t getnoewCSTS = utilities::CRC::crc32i(0, (data.data()+getsuzesoftypes), 4);
    getsuzesoftypes+=sizeof (datarecord.TS1);
    memcpy(data.data()+getsuzesoftypes,&(getnoewCSTS),4);
    data.resize(16);
    return data;
}

QByteArray serial::writerostruct() {
    uint8_t standartcontent[6] { 0x24,0x09,0x08,0x00 };
    //QByteArray data(174,'\0');
    int getsuzesoftypes = sizeof (headrecord.Separatorattribute)+sizeof (headrecord.Namecomand)+sizeof (headrecord.LengthMessage);
    memcpy(data.data(),standartcontent,sizeof (standartcontent));
    memcpy(data.data()+getsuzesoftypes,&headrecord.Indifier,1);
    getsuzesoftypes+=sizeof(headrecord.Indifier);
    uint8_t getnowCS = utilities::CRC::crc8ccitt(0,data.data(),5);
    memcpy(data.data()+getsuzesoftypes,&getnowCS,1);
    getsuzesoftypes+=sizeof (headrecord.ControSum);
    headrecord.Separatorattribute = 0;
    commandsex((namecommands)headrecord.Namecomand)();
    return ret;
}

#include "utils.h"
#include "CRCTABLES.h"

namespace utilities {
namespace ringBuffer {
uint32_t refactspisok(uint8_t * id_list,size_t size) {
    QList<qint32> copydigit;
    for (int i=0;i<size;i++,(id_list)++)
        copydigit.append(*id_list);
    if (size>2) {
        return (copydigit[0]<<24)+(copydigit[1]<<16)+(copydigit[2]<<8)+(copydigit[3]);
    }
    else return (copydigit[1]<<8)+(copydigit[0]);
}
off_t min(off_t a,off_t b){
    return a<b?a:b;
}

QString put(utilities::types::ringBuffer::queue_t *q,QByteArray ba,size_t size) {
    char*data = ba.data();
    if (q->buffer_size - q->bytes_avail<size) return nullptr;
    const size_t part1 = min(size,q->buffer_size-q->tail);
    const size_t part2 = size-part1;
    memcpy(q->buffer+q->tail,data,part1);
    memcpy(q->buffer,(uint8_t *)data+part1,part2);
    q->tail = (q->tail+size)%q->buffer_size;
    q->bytes_avail+=size;
    QString s = QString(ba.toHex());
    return s.remove(' ');
}


uint32_t get(utilities::types::ringBuffer::queue_t*q,utilities::types::HEADing*headrecord) {
    uint32_t data = 0;
    for (int i=3;i>=0;i--) {
        data = data<<8;
        data&=0xffffff00;
        data += q->buffer[(q->head+headrecord->LengthMessage+i-4)%q->buffer_size];
    }
    return data;

}

bool get(utilities::types::ringBuffer::queue_t *q,uint8_t*data,size_t size) {
    if (q->bytes_avail<size) size = q->bytes_avail;
    for (size_t i=0;i<size;i++) {
        data[i] = q->buffer[(q->head+i)%q->buffer_size];
    }
    q->head = (q->head+size) %q->buffer_size;
    q->bytes_avail-=size;
    return true;
}
uint32_t get(utilities::types::ringBuffer::queue_t *q,size_t size) {
    if (q->bytes_avail<size) size = q->bytes_avail;
    uint8_t data1[4];
    for (size_t i=0;i<size;i++) {
        data1[i] = q->buffer[(q->head+i)%q->buffer_size];
    }
    q->head=(q->head+size) %q->buffer_size;
    q->bytes_avail-=size;
    if (size==1) return *data1; else return refactspisok(data1,size);
}


}
namespace CRC {
uint8_t crc8ccitt(uint8_t crc, const void * data, uint32_t size,utilities::types::ringBuffer::queue_t*q) {
    uint8_t val = crc;

    uint8_t * pos = (uint8_t *) data;
    uint8_t * end = pos + size;
    bool checkcrc = false;

    while (pos < end) {
        if (pos-q->buffer>=q->buffer_size&&!checkcrc) {
            pos-=q->buffer_size;
            end-=q->buffer_size;
            checkcrc = true;
        }
        val = CRC_TABLE[val ^ *pos];
        pos++;
    }

    return val;
}
uint8_t crc8ccitt(uint8_t crc, const void * data, uint32_t size) {
    uint8_t val = crc;

    uint8_t * pos = (uint8_t *) data;
    uint8_t * end = pos + size;

    while (pos < end) {

        val = CRC_TABLE[val ^ *pos];
        pos++;
    }

    return val;
}
uint32_t crc32i(unsigned int crc, const void*data, uint32_t lbuf)
{
    uint8_t *pbuf = (uint8_t *) data;
    crc ^= 0xFFFFFFFF;

    while ( lbuf-- )
    {

        crc = CRC32TAB[ (crc & 0xFF) ^ *pbuf++ ] ^ (crc >> 8);

    }
    return crc ^ 0xFFFFFFFF;
}

uint32_t crc32i(unsigned int crc, unsigned char *pbuf, uint32_t lbuf,utilities::types::ringBuffer::queue_t*q)
{
    crc ^= 0xFFFFFFFF;
    uint8_t counter = 0;
    bool checkcrc = false;
    while ( lbuf-- )
    {
        if (q->head+counter>=q->buffer_size&&!checkcrc) {
            pbuf-=q->buffer_size;
            checkcrc = true;
        }
        crc = CRC32TAB[ (crc & 0xFF) ^ *pbuf++ ] ^ (crc >> 8);
        counter++;
    }
    return crc ^ 0xFFFFFFFF;
}
}
}

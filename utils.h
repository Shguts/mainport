#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <QString>
#include <QList>


namespace utilities{
namespace types {
struct HEADing {
    uint8_t Separatorattribute;
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
namespace ringBuffer {
typedef struct {
uint8_t*buffer;
size_t buffer_size;
size_t head;
size_t tail;
size_t bytes_avail;
} queue_t;
}
}
namespace ringBuffer {
QString put(types::ringBuffer::queue_t *q,QByteArray ba,size_t size);
uint32_t get(types::ringBuffer::queue_t*q,types::HEADing*headrecord);
bool get(types::ringBuffer::queue_t *q,uint8_t*data,size_t size);
uint32_t get(types::ringBuffer::queue_t *q,size_t size);
off_t min(off_t a,off_t b);
uint32_t refactspisok(uint8_t * id_list,size_t size);

}
namespace CRC {
  uint8_t crc8ccitt(uint8_t crc, const void * data, uint32_t size,types::ringBuffer::queue_t*q);
  uint8_t crc8ccitt(uint8_t crc, const void * data, uint32_t size);
  uint32_t crc32i(unsigned int crc, const void*data, uint32_t lbuf);
  uint32_t crc32i(unsigned int crc, unsigned char *pbuf, uint32_t lbuf,types::ringBuffer::queue_t*q);
}
}
#endif // UTILS_H

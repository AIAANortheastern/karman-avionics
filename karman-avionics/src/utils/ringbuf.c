/* \file implementation of ring buffer 
 *
 *
 * Andrew Kaster 
 *
 *
 */
#include "ringbuf.h"
#include <string.h>

bool ringbuf_isempty(volatile ringbuf_t *rbuf)
{
    uint32_t count;
    count = rbuf->count;
    return ( count == 0);
}
bool ringbuf_isfull(volatile ringbuf_t *rbuf)
{
    uint32_t count;
    uint32_t size;

    count = rbuf->count;
    size = rbuf->size;

    return (count >= size);
}

uint32_t ringbuf_getcount(volatile ringbuf_t *rbuf)
{
    uint32_t count;

    count = rbuf->count;

    return count;   
}

uint32_t ringbuf_peek(volatile ringbuf_t *rbuf)
{
    uint32_t value;

    value = rbuf->buf[rbuf->tail];

    return value;
}

uint32_t ringbuf_get(volatile ringbuf_t *rbuf, uint8_t *getbuf, uint32_t numbytes)
{
    uint32_t bytescopied;
    /* Check how many bytes are available, then copy to getbuf */
    if((rbuf->count) > 0)
    {
        if((rbuf->count) >= numbytes)
        {
            bytescopied = numbytes;
        }
        else
        {
            bytescopied = rbuf->count;
        }

        memcpy((void *)getbuf, (void *)(&(rbuf->buf[rbuf->tail])), bytescopied);
        rbuf->tail = (rbuf->tail + bytescopied) % (rbuf->size);
        rbuf->count -= bytescopied;
    }
    else
    {
        bytescopied = -1;
    }
    return bytescopied;
}

void ringbuf_put(volatile ringbuf_t *rbuf, uint8_t val)
{
    if((rbuf->count) < (rbuf->size))
    {
        rbuf->buf[rbuf->head] = val;
        rbuf->head = (rbuf->head + 1) % rbuf->size;
        rbuf->count++;
    }
    return;
}

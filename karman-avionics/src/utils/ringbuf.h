/* \file ringbuffer implementation
 *
 * This implementation allows for single byte writes and multibyte reads 
 *
 * Andrew Kaster
 */

#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include "Board.h"

/* NOTE: Do NOT reorder the members of these types! buf MUST be at the end in order for casting */
/* to ringbuf_t to work properly. This is intended! the size member helps protect against overflow */

#define RINGBUF_DECL(prefix, bufsize) \
   typedef struct prefix##_ringbuf_s  \
   {                                  \
        uint32_t head;                \
        uint32_t tail;                \
        uint32_t count;               \
        uint32_t size;                \
        uint8_t  buf[bufsize];        \
   } prefix##_ringbuf_t;              \
                                             \
   inline void prefix##_ringbuf_init(volatile prefix##_ringbuf_t *rbuf)     \
   {                                                        \
        rbuf->size = bufsize;                               \
   }                                                        \


typedef struct
{
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    uint32_t size;
    uint8_t  *buf;
} ringbuf_t;

bool ringbuf_isempty(volatile ringbuf_t *rbuf);
bool ringbuf_isfull(volatile ringbuf_t *rbuf);
uint32_t ringbuf_getcount(volatile ringbuf_t *rbuf);
uint32_t ringbuf_peek(volatile ringbuf_t *rbuf);

/* Returns how many bytes were extracted from the ring buffer */
uint32_t ringbuf_get(volatile ringbuf_t *rbuf, uint8_t *getbuf, uint32_t numbytes);
void ringbuf_put(volatile ringbuf_t *rbuf, uint8_t val);

#endif /* ringbuf.h */

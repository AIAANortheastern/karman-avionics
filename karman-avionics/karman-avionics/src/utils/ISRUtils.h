/*
 * ISRUtils.h
 *
 * Created: 3/16/2017 8:04:38 PM
 *  Author: Andrew Kaster
 */ 


#ifndef ISRUTILS_H_
#define ISRUTILS_H_

#include <asf.h>

static inline uint32_t isrutils_check_shared_u32( volatile uint32_t *var)
{
    uint32_t varVal;
    irqflags_t flags = cpu_irq_save();
    varVal = *var;
    cpu_irq_restore(flags);
    return varVal;
}


#endif /* ISRUTILS_H_ */
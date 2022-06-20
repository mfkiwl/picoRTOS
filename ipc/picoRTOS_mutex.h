#ifndef PICORTOS_MUTEX_H
#define PICORTOS_MUTEX_H

#include "picoRTOS.h"

struct picoRTOS_mutex {
    picoRTOS_atomic_t owner;
    size_t count;
};

/* macro for static init */
#define PICORTOS_MUTEX_INITIALIZER              \
    { (picoRTOS_atomic_t)-1, (size_t)0 }

void picoRTOS_mutex_init(/*@notnull@*/ /*@out@*/ struct picoRTOS_mutex *mutex);         /* init mutex */

/*@unused@*/ int picoRTOS_mutex_trylock(/*@notnull@*/ struct picoRTOS_mutex *mutex);    /* attempt to acquire mutex */
void picoRTOS_mutex_lock(/*@notnull@*/ struct picoRTOS_mutex *mutex);                   /* acquire mutex */
void picoRTOS_mutex_unlock(/*@notnull@*/ struct picoRTOS_mutex *mutex);                 /* release mutex */

#endif

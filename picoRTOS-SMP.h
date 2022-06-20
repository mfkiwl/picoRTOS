#ifndef PICORTOS_SMP_H
#define PICORTOS_SMP_H

/* SMP extensions for picoRTOS */

#define PICORTOS_SMP

#include "picoRTOS.h"

/* CORES */

#ifndef CONFIG_SMP_CORES
# define CONFIG_SMP_CORES 1
#endif

#define PICORTOS_SMP_CORE_ANY                           \
    (picoRTOS_mask_t)((1u << CONFIG_SMP_CORES) - 1u)

/* TASKS */

void picoRTOS_SMP_set_core_mask(picoRTOS_priority_t prio,
                                picoRTOS_mask_t core_mask);

#endif

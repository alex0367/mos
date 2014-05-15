#ifndef _PS0_H_
#define _PS0_H_

/* EFLAGS Register. */
#define FLAG_MBS  0x00000002    /* Must be set. */
#define FLAG_IF   0x00000200    /* Interrupt Flag. */


#define _USER \
    __attribute__((section(".user")))

#define _USERDATA \
    __attribute__((section(".userdata")))

_USER void ps_first_process_run();

#endif

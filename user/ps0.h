#ifndef _PS0_H_
#define _PS0_H_

#define _USER \
    __attribute__((section(".user")))

#define _USERDATA \
    __attribute__((section(".userdata")))

_USER void ps_first_process_run();

#endif

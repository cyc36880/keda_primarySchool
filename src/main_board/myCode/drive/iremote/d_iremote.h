#ifndef __D_IREMOTE_H__
#define __D_IREMOTE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"
#include "iremote.h"

void d_iremote_init(void);
void d_iremote_reset(void);

extern iremote_t iremote;

#ifdef __cplusplus
}
#endif

#endif /* __D_IREMOTE_H__ */

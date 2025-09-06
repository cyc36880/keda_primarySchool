#ifndef __MYMAIN_H__
#define __MYMAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "component/udcheck/udc.h"
#include "component/zs_tools/src/zst_core.h"

#define LOCK   1
#define UNLOCK 0

void setup(void);
void loop(void);

#ifdef __cplusplus
}
#endif

#endif /* __MYMAIN_H__ */

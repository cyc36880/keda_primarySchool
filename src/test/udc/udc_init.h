#ifndef __FILENAME_H__
#define __FILENAME_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "udc.h"

#define UDC_PACK_HEADER    "KX0"
#define UDC_PACK_HEADER_LEN 3

void udc_init(void);

extern udc_pack_t pack;

#ifdef __cplusplus
}
#endif


#endif /* __FILENAME_H__ */


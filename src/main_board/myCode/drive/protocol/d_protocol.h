#ifndef __D_PROTOCOL_H__
#define __D_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"
#include "misc/data_structure.h"

#define PACK_IDENTIFY_ID       100
#define PACK_EXPAND_COMMAND_ID 101

enum _PROTOCOL_ACK
{
    PROTOCOL_ACK_OK = 0,
    PROTOCOL_ACK_MISSING_ID_100,       // 缺少id为100的数据包
    PROTOCOL_ACK_ID_100_NOSUPPORT_CMD, // id为100的数据包，不支持的命令
    PROTOCOL_ACK_SIZE_ERROR,           // 数据包长度错误
    PROTOCOL_ACK_ERROR,                // 其他错误
    PROTOCOL_ASC_INTERNAL_ERROR,       // 协议内部错误
};

void d_protocol_init(void);

extern udc_pack_t protocol_pack_KX0;
extern data_pack_t protocol_data_pack_KX0 ;



#ifdef __cplusplus
}
#endif

#endif /* __D_PROTOCOL_H__ */

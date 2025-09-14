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

/**
 * 数据包必须包含一个id为`100`的数据包，用于判断该数据包的类型。长度为1，后面的数据分别为：
 * 0： 响应。再包含一个id为1的数据包表示响应信息，长度为1
 * 1： 主机给予命令。再包含多个id（目标group），第一个数据表示group的内部地址，后面是数据
 * 2： 主机请求数据。再包含多个id（目标group），数据分别为group的内部地址
 * 3： 主机订阅。后面包含的id表示要订阅的group，长度为0表示要订阅group下所有设备，否则按照给与的地址订阅
 * 4： 主机取消订阅。后面包含的id表示要取消订阅的group，长度为0表示要取消订阅group下所有设备，否则按照给与的地址取消订阅
 * 10: 从机请求数据回应。再包含多个id（目标group），第一个数据表示group的内部地址，第二个数据表示长度，后面是数据
 * 11: 从机订阅回应。再包含多个id（目标group），第一个数据表示group的内部地址，第二个数据表示长度，后面是数据
 * 
 * 包含id为`101`的数据包为主机扩展命令：
 * 长度为1：
 * 0：不要响应（缺省）
 * 1：要响应
 * 
 * 
 */



#ifdef __cplusplus
}
#endif

#endif /* __D_PROTOCOL_H__ */

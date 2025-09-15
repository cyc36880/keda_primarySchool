#ifndef __MYMATH_H__
#define __MYMATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myMain.h"

/**
 * 数值映射
 * 特别注意！！
 *  [0 - 100] -> [0 - 20]
 * 实际上，是把[0 - 99] 平均映射到了 [0-19]
 * 在100时为20，要实现预想的映射，应该为 [0-101] [0-21]
 */
size_t number_map(size_t x, size_t in_min, size_t in_max, size_t out_min, size_t out_max);

#ifdef __cplusplus
}
#endif

#endif /* __MYMATH_H__ */

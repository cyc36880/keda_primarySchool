#include "myMath.h"


size_t number_map(size_t x, size_t in_min, size_t in_max, size_t out_min, size_t out_max) 
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int32_t min(int32_t val1, int32_t val2)
{
    return val1 < val2 ? val1 : val2;
}

int32_t max(int32_t val1, int32_t val2)
{
    return val1 > val2 ? val1 : val2;
}

int32_t myabs(int32_t val)
{
    return val < 0 ? -val : val;
}


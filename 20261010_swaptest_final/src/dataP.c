#include "data_table.h"

const unsigned long data_padding_P[128] = {0};
unsigned long data_padding_P_touch(unsigned long x)
{
    return data_padding_P[x & 127];
}

const unsigned long dataP[128] = { DATA_TABLE_VALUES };

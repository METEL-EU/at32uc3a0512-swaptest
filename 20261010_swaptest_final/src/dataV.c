#include "data_table.h"

const unsigned long data_padding_V[128] = {0};
unsigned long data_padding_V_touch(unsigned long x)
{
    return data_padding_V[x & 127];
}

const unsigned long dataV[128] = { DATA_TABLE_VALUES };

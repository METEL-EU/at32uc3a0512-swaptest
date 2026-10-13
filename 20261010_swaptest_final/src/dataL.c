#include "data_table.h"

const unsigned long data_padding_L[128] = {0};
unsigned long data_padding_L_touch(unsigned long x)
{
    return data_padding_L[x & 127];
}

const unsigned long dataL[128] = { DATA_TABLE_VALUES };

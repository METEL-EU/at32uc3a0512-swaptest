#include "data_table.h"

const unsigned long data_padding_T[128] = {0};
unsigned long data_padding_T_touch(unsigned long x)
{
    return data_padding_T[x & 127];
}

const unsigned long dataT[128] = { DATA_TABLE_VALUES };

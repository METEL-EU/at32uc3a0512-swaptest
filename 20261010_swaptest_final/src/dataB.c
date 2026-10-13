#include "data_table.h"

const unsigned long data_padding_B[128] = {0};
unsigned long data_padding_B_touch(unsigned long x)
{
    return data_padding_B[x & 127];
}

const unsigned long dataB[128] = { DATA_TABLE_VALUES };

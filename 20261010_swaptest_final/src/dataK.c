#include "data_table.h"

const unsigned long data_padding_K[128] = {0};
unsigned long data_padding_K_touch(unsigned long x)
{
    return data_padding_K[x & 127];
}

const unsigned long dataK[128] = { DATA_TABLE_VALUES };

#include "data_table.h"

const unsigned long data_padding_G[128] = {0};
unsigned long data_padding_G_touch(unsigned long x)
{
    return data_padding_G[x & 127];
}

const unsigned long dataG[128] = { DATA_TABLE_VALUES };

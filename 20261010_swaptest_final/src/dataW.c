#include "data_table.h"

const unsigned long data_padding_W[128] = {0};
unsigned long data_padding_W_touch(unsigned long x)
{
    return data_padding_W[x & 127];
}

const unsigned long dataW[128] = { DATA_TABLE_VALUES };

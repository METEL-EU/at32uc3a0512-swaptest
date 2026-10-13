#include "data_table.h"

const unsigned long data_padding_N[128] = {0};
unsigned long data_padding_N_touch(unsigned long x)
{
    return data_padding_N[x & 127];
}

const unsigned long dataN[128] = { DATA_TABLE_VALUES };

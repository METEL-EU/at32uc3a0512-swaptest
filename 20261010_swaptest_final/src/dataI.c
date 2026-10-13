#include "data_table.h"

const unsigned long data_padding_I[128] = {0};
unsigned long data_padding_I_touch(unsigned long x)
{
    return data_padding_I[x & 127];
}

const unsigned long dataI[128] = { DATA_TABLE_VALUES };

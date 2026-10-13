#include "data_table.h"

const unsigned long data_padding_C[128] = {0};
unsigned long data_padding_C_touch(unsigned long x)
{
    return data_padding_C[x & 127];
}

const unsigned long dataC[128] = { DATA_TABLE_VALUES };

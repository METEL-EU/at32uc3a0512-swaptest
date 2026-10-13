#include "data_table.h"

const unsigned long data_padding_E[128] = {0};
unsigned long data_padding_E_touch(unsigned long x)
{
    return data_padding_E[x & 127];
}

const unsigned long dataE[128] = { DATA_TABLE_VALUES };

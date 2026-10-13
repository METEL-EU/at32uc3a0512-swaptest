#include "data_table.h"

const unsigned long data_padding_J[128] = {0};
unsigned long data_padding_J_touch(unsigned long x)
{
    return data_padding_J[x & 127];
}

const unsigned long dataJ[128] = { DATA_TABLE_VALUES };

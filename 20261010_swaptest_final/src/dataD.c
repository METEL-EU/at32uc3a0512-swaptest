#include "data_table.h"

const unsigned long data_padding_D[128] = {0};
unsigned long data_padding_D_touch(unsigned long x)
{
    return data_padding_D[x & 127];
}

const unsigned long dataD[128] = { DATA_TABLE_VALUES };

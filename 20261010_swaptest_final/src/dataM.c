#include "data_table.h"

const unsigned long data_padding_M[128] = {0};
unsigned long data_padding_M_touch(unsigned long x)
{
    return data_padding_M[x & 127];
}

const unsigned long dataM[128] = { DATA_TABLE_VALUES };

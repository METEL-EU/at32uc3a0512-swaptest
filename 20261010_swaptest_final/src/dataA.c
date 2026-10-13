#include "data_table.h"

const unsigned long data_padding_A[128] = {0};
unsigned long data_padding_A_touch(unsigned long x)
{
    return data_padding_A[x & 127];
}

const unsigned long dataA[128] = { DATA_TABLE_VALUES };

#include "data_table.h"

const unsigned long data_padding_U[128] = {0};
unsigned long data_padding_U_touch(unsigned long x)
{
    return data_padding_U[x & 127];
}

const unsigned long dataU[128] = { DATA_TABLE_VALUES };

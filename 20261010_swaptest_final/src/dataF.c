#include "data_table.h"

const unsigned long data_padding_F[128] = {0};
unsigned long data_padding_F_touch(unsigned long x)
{
    return data_padding_F[x & 127];
}

const unsigned long dataF[128] = { DATA_TABLE_VALUES };

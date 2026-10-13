#include "data_table.h"

const unsigned long data_padding_R[128] = {0};
unsigned long data_padding_R_touch(unsigned long x)
{
    return data_padding_R[x & 127];
}

const unsigned long dataR[128] = { DATA_TABLE_VALUES };

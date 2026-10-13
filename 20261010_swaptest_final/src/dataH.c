#include "data_table.h"

const unsigned long data_padding_H[128] = {0};
unsigned long data_padding_H_touch(unsigned long x)
{
    return data_padding_H[x & 127];
}

const unsigned long dataH[128] = { DATA_TABLE_VALUES };

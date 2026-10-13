#include "data_table.h"

const unsigned long data_padding_O[128] = {0};
unsigned long data_padding_O_touch(unsigned long x)
{
    return data_padding_O[x & 127];
}

const unsigned long dataO[128] = { DATA_TABLE_VALUES };

#include "data_table.h"

const unsigned long data_padding_S[128] = {0};
unsigned long data_padding_S_touch(unsigned long x)
{
    return data_padding_S[x & 127];
}

const unsigned long dataS[128] = { DATA_TABLE_VALUES };

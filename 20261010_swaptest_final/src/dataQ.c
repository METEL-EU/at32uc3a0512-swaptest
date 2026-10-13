#include "data_table.h"

const unsigned long data_padding_Q[128] = {0};
unsigned long data_padding_Q_touch(unsigned long x)
{
    return data_padding_Q[x & 127];
}

const unsigned long dataQ[128] = { DATA_TABLE_VALUES };

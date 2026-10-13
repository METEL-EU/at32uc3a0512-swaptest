#include "data_table.h"

const unsigned long data_padding_X[128] = {0};
unsigned long data_padding_X_touch(unsigned long x)
{
    return data_padding_X[x & 127];
}

const unsigned long dataX[128] = { DATA_TABLE_VALUES };

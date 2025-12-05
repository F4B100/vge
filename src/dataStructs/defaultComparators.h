//
// Created by Fabio on 16/11/2025.
//

#ifndef TRIMUN_DEFAULTCOMPARATORS_H
#define TRIMUN_DEFAULTCOMPARATORS_H

#include "typeDefs.h"

int8 uint8Comparator(void *item1, void*item2);
int8 int8Comparator(void *item1, void*item2);
int8 uint16Comparator(void *item1, void*item2);
int8 int16Comparator(void *item1, void*item2);
int8 uint32Comparator(void *item1, void*item2);
int8 int32Comparator(void *item1, void*item2);
int8 uint64Comparator(void *item1, void*item2);
int8 int64Comparator(void *item1, void*item2);
int8 fp32Comparator(void *item1, void*item2);
int8 fp64Comparator(void *item1, void*item2);
int8 stringComparator(void *item1, void*item2);

#endif //TRIMUN_DEFAULTCOMPARATORS_H

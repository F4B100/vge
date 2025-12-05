//
// Created by Fabio on 16/11/2025.
//

#include "defaultComparators.h"

int8 uint8Comparator(void *item1, void*item2) {
    if (*(uint8 *)item1 == *(uint8 *)item2) {
        return 0;
    }
    if (*(uint8 *)item1 > *(uint8 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 int8Comparator(void *item1, void*item2) {
    if (*(int8 *)item1 == *(int8 *)item2) {
        return 0;
    }
    if (*(int8 *)item1 > *(int8 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 uint16Comparator(void *item1, void*item2) {
    if (*(uint16 *)item1 == *(uint16 *)item2) {
        return 0;
    }
    if (*(uint16 *)item1 > *(uint16 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 int16Comparator(void *item1, void*item2) {
    if (*(int16 *)item1 == *(int16 *)item2) {
        return 0;
    }
    if (*(int16 *)item1 > *(int16 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 uint32Comparator(void *item1, void*item2) {
    if (*(uint32 *)item1 == *(uint32 *)item2) {
        return 0;
    }
    if (*(uint32 *)item1 > *(uint32 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 int32Comparator(void *item1, void*item2) {
    if (*(int32 *)item1 == *(int32 *)item2) {
        return 0;
    }
    if (*(int32 *)item1 > *(int32 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 uint64Comparator(void *item1, void*item2) {
    if (*(uint64 *)item1 == *(uint64 *)item2) {
        return 0;
    }
    if (*(uint64 *)item1 > *(uint64 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 int64Comparator(void *item1, void*item2) {
    if (*(int64 *)item1 == *(int64 *)item2) {
        return 0;
    }
    if (*(int64 *)item1 > *(int64 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 fp32Comparator(void *item1, void*item2) {
    if (*(fp32 *)item1 == *(fp32 *)item2) {
        return 0;
    }
    if (*(fp32 *)item1 > *(fp32 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 fp64Comparator(void *item1, void*item2) {
    if (*(fp64 *)item1 == *(fp64 *)item2) {
        return 0;
    }
    if (*(fp64 *)item1 > *(fp64 *)item2) {
        return 1;
    } else {
        return -1;
    }
}
int8 stringComparator(void *item1, void*item2) {
    return strcmp((string)item1, *(string*)item2);
}

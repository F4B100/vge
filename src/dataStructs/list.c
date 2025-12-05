//
// Created by Fabio on 06/11/2025.
//
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * this function initializes a dynamically
 * allocated list of a user determined structure
 * @param sizeElem size of the structure to be held in the list
 * @param numElems number of elements to be allocated initially
 * @return pointer to the start of the list
 */
void *ListAlloc(int64 sizeElem, int64 numElems) {
    if (numElems < 0) {
        fprintf(stderr, "ListError: numElems < 0\n");
        return NULL;
    }
    pListHeader header = malloc(listSizeAlloc(sizeElem,numElems));
    if (!header) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    header->sizeElem = sizeElem;
    header->numElems = numElems;
    if (numElems == 0) {
        header->capacity = 1;
    } else {
        header->capacity = numElems;
    }
    return getListFromHeader(header);
}

/**
 * resizes and frees elements in a lists then returns it
 * @param list pointer to the list to be resized
 * @param elemNum number of elements for the list to have after resize
 * @param freeElem function to be used for freeing the structures that point to other areas in memory
 * @return resized list (may be the same if smaller or same size)
 */
void *ListResize(void *list, int64_t elemNum, void(*freeElem)(void *)) {
    pListHeader header = getHeaderFromList(list);

    uint64_t oldNumElems = header->numElems;
    if (header->numElems < elemNum) {
        header->numElems = 0;
    } else {
        header->numElems -= elemNum;
    }
    if (freeElem != nullptr) {
        for (int i = oldNumElems - 1; i >= header->numElems; i--) {
            freeElem((byte *)list + i * header->sizeElem);
        }
    }

    uint8_t resizeNeeded = header->numElems > header->capacity || header->numElems < (header->capacity / LIST_DEFAULT_GROWTH_FACTOR);
    while(header->numElems > header->capacity) {
        header->capacity *= LIST_DEFAULT_GROWTH_FACTOR;
    }
    while(header->numElems < (header->capacity / LIST_DEFAULT_GROWTH_FACTOR)) {
        header->capacity /= LIST_DEFAULT_GROWTH_FACTOR;
    }
    if(resizeNeeded) {
        header = realloc(header, listSizeAlloc(header->sizeElem,header->capacity));
        return getListFromHeader(header);
    }
    return getListFromHeader(header);
}

/**
 * adds elements in a lists then returns it
 * @param list pointer to the list to be resized
 * @param numElems number of elements for the list to have after resize
 * @return resized list (same if 0 numElems)
 */
void *ListExtend(void *list, uint64_t numElems) {
    pListHeader header = getHeaderFromList(list);

    header->numElems += numElems;
    if(header->numElems > header->capacity) {
        while(header->numElems > header->capacity) {
            header->capacity *= LIST_DEFAULT_GROWTH_FACTOR;
        }
        header = realloc(header, listSizeAlloc(header->sizeElem,header->capacity));
        return getListFromHeader(header);
    }
    return getListFromHeader(header);
}

/**
 * resizes elements in a lists then returns it
 * @param list pointer to the list to be resized
 * @param numElems number of elements for the list to have after resize
 * @param freeElem function to be called when freeing elems that point to other addresses in the heap
 * @return resized list (same if 0 numElems)
 */
void *ListContract(void *list, int64_t numElems, void (*freeElem)(void *)) {
    pListHeader header = getHeaderFromList(list);
    int64_t oldNumElems = header->numElems;
    if (header->numElems < numElems) {
        header->numElems = 0;
    } else {
        header->numElems -= numElems;
    }
    if (freeElem != nullptr) {
        for (int i = oldNumElems - 1; i >= header->numElems; i--) {
            freeElem((byte *)list + i * header->sizeElem);
        }
    }

    uint8_t resizeNeeded = header->numElems < (header->capacity / LIST_DEFAULT_GROWTH_FACTOR);
    while(header->numElems < (header->capacity / LIST_DEFAULT_GROWTH_FACTOR)) {
        header->capacity /= LIST_DEFAULT_GROWTH_FACTOR;
    }
    if (header->capacity <= 0) {
        header->capacity = 1;
    }
    if(resizeNeeded) {
        header = realloc(header, listSizeAlloc(header->sizeElem,header->capacity));
        return getListFromHeader(header);
    }
    return getListFromHeader(header);
}

/**
 * frees the memory used by a list
 * @param list pointer to the list to be freed
 * @param freeElem function to be called on all elements that point to other addresses on the heap
 */
void ListFree(void *list, void (*freeElem)(void *)) {
    if (freeElem != nullptr) {
        foreach(void *, elem, list) {
            freeElem(elem);
        }
    }
    free(getHeaderFromList(list));
}

int8 stringListComparator(void *item1, void*item2) {
    return strcmp(*(string*)item1, *(string*)item2);
}

int64_t listBinSearch(void *list, void* toCompare, int8_t(*comparator)(void *item1, void *item2)) {
    pListHeader header = getHeaderFromList(list);
    int64_t left = 0, right = header->numElems - 1, mid = left + (right - left) / 2;
    for (; left <= right; mid = left + (right - left) / 2) {
        if (comparator(toCompare, (byte *)list + mid * header->sizeElem) == 0) {
            return mid;
        }

        if (comparator(toCompare, (byte *)list + mid * header->sizeElem) > 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

int64 partition(void *list,int64 low,int64 high, int8 comparator(void *item1, void *item2)) {
    pListHeader header = getHeaderFromList(list);
    void *pivot = high * header->sizeElem + (byte *)list;
    byte temp[header->sizeElem];

    int64 i = low - 1;

    for (int64 j = low; j < high ; j++) {
        if (comparator((byte *)list + j * header->sizeElem, pivot) <= 0) {
            i++;
            memcpy(temp, (byte *)list + i * header->sizeElem, header->sizeElem);
            memcpy((byte *)list + i * header->sizeElem, (byte *)list + j * header->sizeElem, header->sizeElem);
            memcpy((byte *)list + j * header->sizeElem, temp, header->sizeElem);
        }
    }
    memcpy(temp, (byte *)list + (i + 1) * header->sizeElem, header->sizeElem);
    memcpy((byte *)list + (i + 1) * header->sizeElem, (byte *)list + high * header->sizeElem, header->sizeElem);
    memcpy((byte *)list + high * header->sizeElem, temp, header->sizeElem);

    return i + 1;
}

void quicksort(void *list, int64 low, int64 high, int8 comparator(void *item1, void *item2)) {
    if (low < high) {
        int64 pivotIndex = partition(list, low, high, comparator);
        quicksort(list, low, pivotIndex-1, comparator);
        quicksort(list, pivotIndex+1, high, comparator);
    }
}

/**
 *
 * @param list list to be sorted
 * @param comparator function used to compare two elements in the list
 */
void listSort(void *list, int8 comparator(void *item1, void *item2)) {
    if (listGetNumElems(list) <= 1)
        return;
    quicksort(list, 0, listGetNumElems(list) - 1, comparator);
}

void listInsertSorted(void *list, void *toInsert, int8 comparator(void *item1, void *item2)) {
    pListHeader header = getHeaderFromList(list);
    int64_t left = 0, right = header->numElems - 1, mid = left + (right - left) / 2;
    for (; left <= right; mid = left + (right - left) / 2) {
        if (comparator(toInsert, (byte *)list + mid * header->sizeElem) == 0) {
            return ;
        }

        if (comparator(toInsert, (byte *)list + mid * header->sizeElem) > 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

}

//
// Created by Fabio on 06/11/2025.
//
#ifndef VGE_LIST_H
#define VGE_LIST_H
#pragma once

#include "typeDefs.h"
typedef struct ListHeader {
    int64 sizeElem;
    int64 numElems;
    int64 capacity;
} listHeader, *pListHeader;

/**
 * size of the listHeader struct
 */
#define LIST_HEADER_SIZE sizeof(struct ListHeader)
/**
 * gets the list of given listHeader
 * @param header pointer to a valid header
 */
#define getListFromHeader(header) (((byte *)header) + LIST_HEADER_SIZE)
/**
 * gets the header of given list
 * @param list pointer to a valid list
 */
#define getHeaderFromList(list) (pListHeader)(((byte *)list) - LIST_HEADER_SIZE)

/**
 * calculates the size (in bytes) to be allocated to initialize the list (header included)
 * @param sizeElem size of the list elements to be allocated
 * @param numElems number of elements of the list to be allocated
 */
#define listSizeAlloc(sizeElem,numElems) LIST_HEADER_SIZE + sizeElem * numElems
/**
 * gets the size (in bytes) of the list
 */
#define listGetSize(list) ((pListHeader)(((byte *)list) - LIST_HEADER_SIZE))->numElems * ((pListHeader)(list - LIST_HEADER_SIZE))->sizeElem
/**
 * gets the number of elements that should be used by the list
 */
#define listGetNumElems(list) ((pListHeader)(((byte *)list) - LIST_HEADER_SIZE))->numElems
/**
 * gets the number of elements that should be used by the list
 */
#define listGetCapacity(list) ((pListHeader)(((byte *)list) - LIST_HEADER_SIZE))->capacity
/**
 * gets the size if the list elements
 */
#define listGetElemSize(list) ((pListHeader)(((byte *)list) - LIST_HEADER_SIZE))->sizeElem

#define listGetLast(list) (list[listGetNumElems(list) - 1])

/**
 * defines the number(factor) with witch the list grows
 */
#define LIST_DEFAULT_GROWTH_FACTOR 2

void *ListAlloc(int64 sizeElem, int64 numElems);

/**
 * initializes a list of a given type, name and size
 * @param type dataType of the elements of the list to be created
 * @param name name of the variable to hold the list
 * @param size size of the list
 */
#define listAlloc(type, name, size) type* name = ListAlloc(sizeof(type), size)
/**
 * initializes a list of a given type, name and size in a heap allocated memory region
 * @param type dataType of the elements of the list to be created
 * @param name name of the variable to hold the list
 * @param size size of the list
 */
#define listAllocStruct(type, name, size) name = ListAlloc(sizeof(type), size)
/**
 * initializes an empty list of a given type and name
 * @param type dataType of the elements of the list to be created
 * @param name name of the variable to hold the list
 */
#define listAllocEmpty(type, name) type name = ListAlloc(sizeof(type), 0)
/**
 * initializes an empty list of a given type and name in a heap allocated memory region
 * @param type dataType of the elements of the list to be created
 * @param name name of the variable to hold the list
 */
#define listAllocEmptyStruct(type, name) name = ListAlloc(sizeof(type), 0)

/**
 * creates a list of fp32(float) elements
 * @param initialCapacity number of elements of the list
 */
#define fp32List(name, initialCapacity) listAlloc(fp32, name, initialCapacity)
/**
 * creates a list of fp64(double) elements
 * @param initialCapacity number of elements of the list
 */
#define fp64List(name, initialCapacity) listAlloc(fp64, name, initialCapacity)
/**
 * creates a list of int8 elements
 * @param initialCapacity number of elements of the list
 */
#define int8List(name, initialCapacity) listAlloc(int8, name, initialCapacity)
/**
 * creates a list of uint8 elements
 * @param initialCapacity number of elements of the list
 */
#define uint8List(name, initialCapacity) listAlloc(uint8, name, initialCapacity)
/**
 * creates a list of int16 elements
 * @param initialCapacity number of elements of the list
 */
#define int16List(name, initialCapacity) listAlloc(int16, name, initialCapacity)
/**
 * creates a list of uint16 elements
 * @param initialCapacity number of elements of the list
 */
#define uint16List(name, initialCapacity) listAlloc(uint16, name, initialCapacity)
/**
 * creates a list of int32 elements
 * @param initialCapacity number of elements of the list
 */
#define int32List(name, initialCapacity) listAlloc(int32, name, initialCapacity)
/**
 * creates a list of uint32 elements
 * @param initialCapacity number of elements of the list
 */
#define uint32List(name, initialCapacity) listAlloc(uint32, name, initialCapacity)
/**
 * creates a list of uint64 elements
 * @param initialCapacity number of elements of the list
 */
#define int64List(name, initialCapacity) listAlloc(int64, name, initialCapacity)
/**
 * creates a list of uint64 elements
 * @param initialCapacity number of elements of the list
 */
#define uint64List(name, initialCapacity) listAlloc(uint64, name, initialCapacity)

/**
 * a foreach loop loops through all the elements of a list storing tye address of the current element in the var variable
 * @param type type of the elements of the list
 * @param var name of the local variable used to store the address of the current element
 * @param list variable holding the address of the start of the list
 */
#define foreach(type,var,list) for (type *var = list; var < ((type*) list) + listGetNumElems(list); var++)

void *ListResize(void *list, int64 numElems, void(*freeElem)(void *));
/**
 * resizes list
 * @param list pointer to the list to be resized
 * @param numElems number of elements for the list to have after resize
 */
#define listResize(list,numElems) list = ListResize(list,numElems, nullptr);
/**
* resizes and frees elements in a lists
* @param list pointer to the list to be resized
* @param numElems number of elements for the list to have after resize
* @param freeElem function to be used for freeing the structures that point to other areas in memory
*/
#define listResizeRecursive(list,numElems,func) list = ListResize(list,numElems, func)

void *ListExtend(void *list, uint64 numElems);
/**
 * adss elements in a lists
 * @param list pointer to the list to be resized
 * @param numElems number of elements for the list to have after resize
 * @return resized list (same if 0 numElems)
 */
#define listExtend(list,numElems) list = ListExtend(list,numElems)

void *ListContract(void *list, int64 numElems, void (*freeElem)(void *));
/**
 * resizes elements in a lists then returns it
 * @param list pointer to the list to be resized
 * @param numElems number of elements for the list to have after resize
 */
#define listContract(list,numElems) list = ListContract(list,numElems, nullptr)
/**
 * resizes elements in a lists then returns it
 * @param list pointer to the list to be resized
 * @param numElems number of elements for the list to have after resize
 * @param freeElem function to be called when freeing elems that point to other addresses in the heap
 */
#define listContractRecursive(list, numElems, func) list = ListContract(list,numElems, func)

/**
 * removes a range of elements from a list and recursively frees list elements
 * @param list pointer to the list
 * @param startIndex start of range of elements to be removed
 * @param endIndex end of range of elements to be removed
 * @param freeElem function to be called when freeing elems that point to other addresses in the heap
 */
void *ListRemove(void *list, int64 startIndex, int64 endIndex, void (*freeElem)(void *));
/**
 * removes element from a list
 * @param list pointer to the list
 * @param index element to be removed
 */
#define listRemove(list, index) list = ListRemove(list, index, index, nullptr)
/**
 * removes element from a list and calls function to free said element
 * @param list pointer to the list
 * @param index element to be removed
 * @param freeElem function to be called when freeing elems that point to other addresses in the heap
 */
#define listRemoveRecurse(list, index, func) list = ListRemove(list, index, index, func)
/**
 * removes a range of elements from a list
 * @param list pointer to the list
 * @param start start of range of elements to be removed
 * @param end end of range of elements to be removed
 */
#define listRemoveRange(list, start, end) list = ListRemove(list, start, end, nullptr)
/**
 * removes a range of elements from a list and recursively frees list elements
 * @param list pointer to the list
 * @param start start of range of elements to be removed
 * @param end end of range of elements to be removed
 * @param freeElem function to be called when freeing elems that point to other addresses in the heap
 */
#define listRemoveRangeRecurse(list, start, end, func) list = ListRemove(list, start, end, func)

/**
 * frees the memory used by a list
 * @param list pointer to the list to be freed
 * @param freeElem optional function that's called on every element
 */
void ListFree(void *list, void (*freeElem)(void *));
/**
 * frees the memory used by a list
 * @param list pointer to the list to be freed
 */
#define listFree(list) ListFree(list, nullptr)
/**
 * frees the memory used by a list
 * @param list pointer to the list to be freed
 * @param freeElem function to be called on all elements that point to other addresses on the heap
 */
#define listFreeRecursive(list, func) ListFree(list, func)

// ==============================DEFAULT COMPARATORS==============================

#include "defaultComparators.h"
int8 stringListComparator(void *item1, void*item2);

// ===============================================================================

int64 listBinSearch(void *list, void* toCompare, int8(*comparator)(void *item1, void *item2));
/**
 * performs a binary search on a list of uint8 elements
 */
#define listBinSearchUint8(list, toSearch, comparator) listBinSearch(list, toSearch, uint8Comparator)
/**
 * performs a binary search on a list of int8 elements
 */
#define listBinSearchInt8(list, toSearch, comparator) listBinSearch(list, toSearch, int8Comparator)
/**
 * performs a binary search on a list of uint16 elements
 */
#define listBinSearchUint16(list, toSearch, comparator) listBinSearch(list, toSearch, uint16Comparator)
/**
 * performs a binary search on a list of int16 elements
 */
#define listBinSearchInt16(list, toSearch, comparator) listBinSearch(list, toSearch, int16Comparator)
/**
 * performs a binary search on a list of uint32 elements
 */
#define listBinSearchUint32(list, toSearch, comparator) listBinSearch(list, toSearch, uint32Comparator)
/**
 * performs a binary search on a list of int32 elements
 */
#define listBinSearchInt32(list, toSearch, comparator) listBinSearch(list, toSearch, int32Comparator)
/**
 * performs a binary search on a list of uint64 elements
 */
#define listBinSearchUint64(list, toSearch, comparator) listBinSearch(list, toSearch, uint64Comparator)
/**
 * performs a binary search on a list of int64 elements
 */
#define listBinSearchInt64(list, toSearch, comparator) listBinSearch(list, toSearch, int64Comparator)
/**
 * performs a binary search on a list of fp32(float) elements
 */
#define listBinSearchFp32(list, toSearch, comparator) listBinSearch(list, toSearch, fp32Comparator)
/**
 * performs a binary search on a list of fp64(double) elements
 */
#define listBinSearchFp64(list, toSearch, comparator) listBinSearch(list, toSearch, fp64Comparator)
/**
 * performs a binary search on a list of string(char*) elements
 */
#define listBinSearchString(list, toSearch, comparator) listBinSearch(list, toSearch, stringComparator)

/**
 *
 * @param list list to be sorted
 * @param comparator function used to compare the elements of the list
 */
void listSort(void *list, int8 comparator(void *item1, void *item2));
/**
 * sorts a list of uint8 elements
 */
#define listSortUint8(list) listSort(list, uint8Comparator)
/**
 * sorts a list of int8 elements
 */
#define listSortInt8(list) listSort(list, int8Comparator)
/**
 * sorts a list of uint16 elements
 */
#define listSortUint16(list) listSort(list, uint16Comparator)
/**
 * sorts a list of int16 elements
 */
#define listSortInt16(list) listSort(list, int16Comparator)
/**
 * sorts a list of uint32 elements
 */
#define listSortUint32(list) listSort(list, uint32Comparator)
/**
 * sorts a list of int32 elements
 */
#define listSortInt32(list) listSort(list, int32Comparator)
/**
 * sorts a list of uint64 elements
 */
#define listSortUint64(list) listSort(list, uint64Comparator)
/**
 * sorts a list of int64 elements
 */
#define listSortInt64(list) listSort(list, int64Comparator)
/**
 * sorts a list of fp32(float) elements
 */
#define listSortFp32(list) listSort(list, fp32Comparator)
/**
 * sorts a list of fp64(double) elements
 */
#define listSortFp64(list) listSort(list, fp64Comparator)
/**
 * sorts a list of string(char*) elements
 */
#define listSortString(list) listSort(list, stringListComparator)

#endif //VGE_LIST_H
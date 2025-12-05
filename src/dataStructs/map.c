//
// Created by Fabio on 06/11/2025.
//

#include "map.h"

#include <stdlib.h>
#include <string.h>

#include "list.h"

uint64_t hashGenerate(char *data) {
    uint64_t hash = 0x1751;
    for (char *str = data; *str != 0; str++) {
        hash = (hash << 5) + (hash << 3) + (hash) + *str;
    }
    return hash;
}

pHashMap hashMapAlloc() {
    pHashMap map = malloc(sizeof(hashMap));

    listAllocStruct(hashEntry, map->entries, 1);

    return map;
}

int8 hashComparator(void *item1, void *item2) {
    if (*(pUint64)item1 == ((pHashEntry) item2)->hash) {
        return 0;
    }
    if (*(pUint64)item1 > ((pHashEntry) item2)->hash) {
        return 1;
    } else {
        return -1;
    }
}

int8_t hashListComparator(void *item1, void *item2) {
    if (((pHashEntry) item1)->hash == ((pHashEntry) item2)->hash) {
        return 0;
    }
    if (((pHashEntry) item1)->hash > ((pHashEntry) item2)->hash) {
        return 1;
    } else {
        return -1;
    }
}

void *hashMapInsert(pHashMap map, char *key, void* data) {
    uint64 hash = hashGenerate(key);
    int64 hashLocation = listBinSearch(map->entries, &hash, hashComparator);
    if (hashLocation == -1) {
        listExtend(map->entries, 1);
        int64 last = listGetNumElems(map->entries) - 1;

        map->entries[last].hash = hash;
        map->entries[last].localEntries = ListAlloc(sizeof(localHashEntry), 1);

        map->entries[last].localEntries[0].key = strdup(key);
        map->entries[last].localEntries[0].data = data;
    } else {
        int64 hashLocalLocation = listBinSearchString(map->entries[hashLocation].localEntries, key, comparator);
        if (hashLocalLocation == -1) {
            listExtend(map->entries[hashLocation].localEntries, 1);
            int64 last = listGetNumElems(map->entries[hashLocation].localEntries) - 1;
            map->entries[hashLocation].localEntries[last].key = strdup(key);
            map->entries[hashLocation].localEntries[last].data = data;
        } else {
            free(map->entries[hashLocation].localEntries[hashLocalLocation].key);
            void *prevData = map->entries[hashLocation].localEntries[hashLocalLocation].data;
            map->entries[hashLocation].localEntries[hashLocalLocation].key = strdup(key);
            map->entries[hashLocation].localEntries[hashLocalLocation].data = data;
            return prevData;
        }
    }
    return nullptr;
}

void *hashMapRemove(pHashMap map, char *key) {
    int64 hash = hashGenerate(key);
    int64 hashLocation = listBinSearch(map->entries, &hash, hashComparator);
    if (hashLocation != -1) {
        int64 hashLocal = listBinSearchString(map->entries[hashLocation].localEntries, key, comparator);
        if (hashLocal != -1) {
            free(map->entries[hashLocation].localEntries[hashLocal].key);
            listRemove(map->entries[hashLocation].localEntries, hashLocal);
        }
    }
    return nullptr;
}

void *hashMapGet(pHashMap map, char *key) {
    int64 hash = hashGenerate(key);
    int64 hashLocation = listBinSearch(map->entries, &hash, hashComparator);
    if (hashLocation != -1) {
        int64 hashLocal =
        listBinSearchString(map->entries[hashLocation].localEntries, key, comparator);
        if (hashLocal != -1) {
            return map->entries[hashLocation].localEntries[hashLocal].data;
        }
    }
    return nullptr;
}

void hashMapFree(pHashMap map) {
    foreach(hashEntry, entry, map->entries) {
        foreach(localHashEntry, localEntry, entry->localEntries) {
            free(localEntry->key);
        }
        listFree(entry->localEntries);
    }
    listFree(map->entries);
}
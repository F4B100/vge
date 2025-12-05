//
// Created by Fabio on 06/11/2025.
//
#pragma once
#ifndef TRIMUN_MAP_H
#define TRIMUN_MAP_H
#include <stdint.h>

#include "typeDefs.h"

typedef struct LocalHashEntry {
    char *key;
    void *data;
} localHashEntry, *pLocalHashEntry;

typedef struct HashEntry {
    uint64_t hash;
    pLocalHashEntry localEntries;
} hashEntry, *pHashEntry;

typedef struct HashMap {
    pHashEntry entries;
} hashMap, *pHashMap;

uint64_t hashGenerate(char *data);
#define DEFAULT_HASH_FUNCTION hashGenerate

pHashMap hashMapAlloc();

void *hashMapInsert(pHashMap map, char *key, void* data);
void *hashMapRemove(pHashMap map, char *key);
void *hashMapGet(pHashMap map, char *key);

void hashMapFree(pHashMap map);

#endif //TRIMUN_MAP_H
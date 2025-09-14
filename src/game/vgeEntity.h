//
// Created by fabio on 9/13/2025.
//

#ifndef VGE_GAMEENTITY_H
#define VGE_GAMEENTITY_H

#include "../utils/vgeThread.h"
#include "vgeMain.h"

typedef struct VgeGameEntity vgeGameEntity, pVgeGameEntity;
// OMG ITS OOP SO GROSS
struct VgeGameEntity {
	vgeMutex entityMutex;
	vgeMutex entityDataMutex;
	vgeCond dataUpdateFinished;
	uint8_t waiting;
	void * gameInfo;
	void * entityData;
	pVgeGameEntity(*start)();
	pVgeGameEntity(*update)(pVgeGameEntity entity);
	void *(*handleMessage)(pVgeGameEntity entity, uint64_t id, void *data);
	pVgeGameEntity(*draw)(pVgeGameEntity entity);
	pVgeGameEntity(*end)(pVgeGameEntity entity);
};

typedef struct VgeGameEntityCreateInfo {
	uint64_t sizeOfData;
	pVgeGameEntity(*start)();
	pVgeGameEntity(*update)(pVgeGameEntity entity);
	void *(*handleMessage)(pVgeGameEntity entity, uint64_t id, void *data);
	pVgeGameEntity(*draw)(pVgeGameEntity entity);
	pVgeGameEntity(*end)(pVgeGameEntity entity);
} vgeGameEntityCreateInfo, *pVgeGameEntityCreateInfo;

void registerEntity(pVgeEngineInstance engine,pVgeGameEntityCreateInfo info);


#endif // VGE_GAMEENTITY_H

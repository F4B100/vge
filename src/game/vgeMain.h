//
// Created by fabio on 22-05-2025.
//

#ifndef GAMEMAIN_H
#define GAMEMAIN_H

#define VGE_USE_VULKAN
#include "../utils/vector/vgeVector.h"

typedef struct VgeEngineInstance {
	pVgeVector gameEntities;
	void *globals;

} vgeEngineInstance, *pVgeEngineInstance;

void startEngine(uint64_t sizeData);
void GameStart(void *info);
void GameLoop(void *info);
uint32_t exitCondition(void *info);
void GameEnd(void *info);

#endif //GAMEMAIN_H

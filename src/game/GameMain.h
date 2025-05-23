//
// Created by fabio on 22-05-2025.
//

#ifndef GAMEMAIN_H
#define GAMEMAIN_H

#include "../graphics/vulkanInit.h"
#include "../window/vgeWindow.h"

typedef struct GameInfo {
    vgeWindow *window;
    double deltaTime;
    uint32_t frameCount;
    double timeElapsed;
} gameInfo;

void GameInit();
void GameStart(gameInfo *info);
void GameLoop(gameInfo *info);

#endif //GAMEMAIN_H

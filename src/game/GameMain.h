//
// Created by fabio on 22-05-2025.
//

#ifndef GAMEMAIN_H
#define GAMEMAIN_H

#define VGE_USE_VULKAN
#include "../graphics/vgeGraphics.h"
#include "../window/vgeWindow.h"

typedef struct GameInfo {
    vgeWindow *window;
    pVulkanContext graphics;
    VgePipelineGraphics *graphicsP;

    uint32_t frameContexCount;
    uint32_t currentFrame;
    frameContext *frameContext;


    double deltaTime;
    uint32_t frameCount;
    double timeElapsed;
} gameInfo;

void GameInit();
void GameStart(gameInfo *info);
void GameLoop(gameInfo *info);
void GameEnd(gameInfo *info);

#endif //GAMEMAIN_H

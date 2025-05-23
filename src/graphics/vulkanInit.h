//
// Created by fabio on 22-05-2025.
//

#ifndef VULKANINIT_H
#define VULKANINIT_H

#include "vulkanDefs.h"

vulkanContext *initVulkan(vgeWindow *window);

void createVulkanInstance(vulkanContext *context);


#endif //VULKANINIT_H

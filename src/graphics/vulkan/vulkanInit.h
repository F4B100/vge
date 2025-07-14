//
// Created by fabio on 22-05-2025.
//

#ifndef VULKANINIT_H
#define VULKANINIT_H

#include "vulkanDefs.h"

pVulkanContext initVulkan(vgeWindow *window);
void destroyVulkan(pVulkanContext context);

#endif //VULKANINIT_H

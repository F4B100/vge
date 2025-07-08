//
// Created by fabio on 21-05-2025.
//

#ifndef VGEWINDOW_H
#define VGEWINDOW_H

#include "vgeWIndowDefs.h"
#include <stdlib.h>

void vgeInit();

vgeWindow *vgeWindowInit(int32_t width, int32_t height, const int8_t *title, GLFWmonitor *monitor, GLFWwindow *share);


#endif //VGEWINDOW_H

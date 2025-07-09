//
// Created by fabio on 7/9/2025.
//

#ifndef VGETHREAD_H
#define VGETHREAD_H
#include <stdint.h>

#ifdef VGE_PLATFORM_WIN32
#include <windows.h>
typedef struct VgeThread_t {
	DWORD idThread;
	HANDLE hThread;
} vgeThread, *pVgeThread;
#elifdef VGE_PLATFORM_WAYLAND

#else

#endif

uint32_t vgeThreadCreate(vgeThread *thread, vgeThreadFunc func, void *arg);

#endif //VGETHREAD_H

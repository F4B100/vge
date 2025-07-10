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

typedef CRITICAL_SECTION vgeMutex, *pVgeMutex;
#elifdef VGE_PLATFORM_WAYLAND

#else

#endif

// Thread funcs
uint32_t vgeThreadCreate(pVgeThread thread, void *(*func)(void *), void *arg);
uint32_t vgeThreadJoin(pVgeThread thread, void **retVal);

// mutexes "Work more like linux"
uint32_t cross_mutex_init(pVgeMutex mutex);
uint32_t cross_mutex_lock(pVgeMutex mutex);
uint32_t cross_mutex_unlock(pVgeMutex mutex);
uint32_t cross_mutex_destroy(pVgeMutex mutex);

// *future* Process funcs


#endif //VGETHREAD_H

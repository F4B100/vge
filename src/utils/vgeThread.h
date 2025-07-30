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
typedef CONDITION_VARIABLE vgeCond, *pVgeCond;
#elifdef VGE_PLATFORM_WAYLAND
#include <pthread.h>
typedef struct VgeThread_t {
	pthread_t idThread;
} vgeThread, *pVgeThread;

typedef pthread_mutex_t vgeMutex, *pVgeMutex;
typedef pthread_cond_t vgeCond, *pVgeCond;
#else

#endif

// Thread funcs
uint32_t vgeThreadCreate(pVgeThread thread, void *(*func)(void *), void *arg);
void vgeThreadJoin(pVgeThread thread, void **retVal);

// mutexes "Work more like linux"
uint32_t vgeMutexInit(pVgeMutex mutex);
uint32_t vgeMutexLock(pVgeMutex mutex);
uint32_t vgeMutexUnlock(pVgeMutex mutex);
uint32_t vgeMutexDestroy(pVgeMutex mutex);

// conditionals "same as above"
uint32_t vgeCondInit(pVgeCond cond);
void vgeCondWait(pVgeCond cond, pVgeMutex mutex);
void vgeCondSignal(pVgeCond cond);
uint32_t vgeCondDestroy(pVgeCond cond);

// *future* Process funcs


#endif //VGETHREAD_H

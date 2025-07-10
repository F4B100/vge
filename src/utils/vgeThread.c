//
// Created by fabio on 7/9/2025.
//

#include "vgeThread.h"

#ifdef VGE_PLATFORM_WIN32

uint32_t vgeThreadCreate(pVgeThread *thread, void *(*func)(void *), void *arg) {
	pVgeThread vgeThread = malloc(sizeof(*vgeThread));

	if (!vgeThread) {
		return 1;

	}
	vgeThread->hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) func, arg, 0, NULL);
	*thread = vgeThread;

	return 0;
}

uint64_t vgeThreadJoin(pVgeThread thread, void **retVal) {
	return WaitForSingleObject(thread->hThread, INFINITE);;
}

uint32_t cross_mutex_init(pVgeMutex mutex) {
	InitializeCriticalSection(mutex);
	return 0;
}

uint32_t cross_mutex_lock(pVgeMutex mutex) {
	EnterCriticalSection(mutex);
	return 0;
}

uint32_t cross_mutex_unlock(pVgeMutex mutex) {
	LeaveCriticalSection(mutex);
	return 0;
}

uint32_t cross_mutex_destroy(pVgeMutex mutex) {
	DeleteCriticalSection(mutex);
	return 0;
}

#elifdef VGE_PLATFORM_WAYLAND



#else



#endif

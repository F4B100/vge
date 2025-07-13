//
// Created by fabio on 7/9/2025.
//

#include "vgeThread.h"

#include <stdio.h>

#ifdef VGE_PLATFORM_WIN32

typedef struct {
	void *(*func)(void *);
	void *arg;
} ThreadStartWrapper;

DWORD vgeThreadStart(LPVOID param) {
	ThreadStartWrapper *wrapper = (ThreadStartWrapper *)param;
	wrapper->func(wrapper->arg);
	free(wrapper);
	return 0;
}

uint32_t vgeThreadCreate(pVgeThread thread, void *(*func)(void *), void *arg) {
	ThreadStartWrapper *wrapper = malloc(sizeof(ThreadStartWrapper));
	if (!wrapper) {
		return 1;
	}

	wrapper->func = func;
	wrapper->arg = arg;

	thread->hThread = CreateThread(NULL, 0, vgeThreadStart, wrapper, 0, NULL);
	if (!thread->hThread) {
		free(wrapper);
		return 2;
	}
	return 0;
}

void vgeThreadJoin(pVgeThread thread, void **retVal) {
	WaitForSingleObject(thread->hThread, INFINITE);
	CloseHandle(thread->hThread);
}

uint32_t vgeMutexInit(pVgeMutex mutex) {
	InitializeCriticalSection(mutex);
	return 0;
}

uint32_t vgeMutexLock(pVgeMutex mutex) {
	EnterCriticalSection(mutex);
	return 0;
}

uint32_t vgeMutexUnlock(pVgeMutex mutex) {
	LeaveCriticalSection(mutex);
	return 0;
}

uint32_t vgeMutexDestroy(pVgeMutex mutex) {
	DeleteCriticalSection(mutex);
	return 0;
}

uint32_t vgeCondInit(pVgeCond cond) {
	InitializeConditionVariable(cond);
	return 0;
}

void vgeCondWait(pVgeCond cond, pVgeMutex mutex) {
	SleepConditionVariableCS(cond, mutex, INFINITE);
}

void vgeCondSignal(pVgeCond cond) {
	WakeConditionVariable(cond);
}

uint32_t vgeCondDestroy(pVgeCond cond) {
	return 0;
}

#elifdef VGE_PLATFORM_WAYLAND



#else



#endif

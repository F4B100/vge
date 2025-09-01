//
// Created by fabio on 8/27/2025.
//

#ifndef VGEVECTOR_H
#define VGEVECTOR_H
#include <stdint.h>

typedef struct VgeVector{
	uint64_t numElements;
	uint64_t sizeElement;
	uint64_t capacity;
	void *data;
} *pVgeVector;

#define VGE_VECTOR_CAPACITY_INITIAL 4
#define VGE_VECTOR_CAPACITY_GROWTH_FACTOR 2.0f
#define VGE_VECTOR_CAPACITY_SHRINK_FACTOR 0.25f

pVgeVector vgeVectorInit(uint64_t sizeElement);
void vgeVectorFree(pVgeVector *vgeVector);
void *vgeVectorFreeKeepData(pVgeVector *vgeVector);

void vgeVectorAppend(pVgeVector vector, void *data);
void vgeVectorInsert(pVgeVector vector, void *data, uint64_t index);
void vgeVectorRemoveLast(pVgeVector vector);
void vgeVectorRemove(pVgeVector vector, uint64_t index);
void *vgeVectorGetData(pVgeVector vgeVector);
void *vgeVectorGetElement(pVgeVector vgeVector, uint64_t index);



#endif //VGEVECTOR_H

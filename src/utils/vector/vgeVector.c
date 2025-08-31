//
// Created by fabio on 8/27/2025.
//

#include "vgeVector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pVgeVector vgeVectorInit(uint64_t sizeElement) {
	pVgeVector vgeVector = malloc(sizeof(vgeVector));
	if (!vgeVector) {
		return NULL;
	}
	vgeVector->data = malloc(sizeElement * sizeof(uint32_t) * VGE_VECTOR_CAPACITY_INITIAL);
	vgeVector->capacity = VGE_VECTOR_CAPACITY_INITIAL;
	vgeVector->sizeElement = sizeElement;
	vgeVector->numElements = 0;
	return vgeVector;
}

void vgeVectorFree(pVgeVector *vgeVector) {
	if (!*vgeVector) {
		return;
	}
	free((*vgeVector)->data);
	free(*vgeVector);
	*vgeVector = nullptr;
}

void *vgeVectorFreeKeepData(pVgeVector *vgeVector) {
	if (!*vgeVector) {
		return NULL;
	}
	void *data = (*vgeVector)->data;
	free(*vgeVector);
	*vgeVector = nullptr;
	return data;
}

void vgeVectorAppend(pVgeVector vector, void *data) {
	if (!vector || !data) {
		return;
	}
	vector->numElements++;
	if (vector->numElements > vector->capacity) {
		vector->capacity *= VGE_VECTOR_CAPACITY_GROWTH_FACTOR;
		printf("vector size:%lld", vector->capacity * vector->sizeElement);
		vector->data = realloc(vector->data, vector->capacity * vector->sizeElement);
	}
	memcpy(vector->data + (vector->numElements - 1) * vector->sizeElement, data, vector->sizeElement);
}

void vgeVectorInsert(pVgeVector vector, void *data, uint64_t index) {
	if (!vector || !data || index >= vector->numElements) {
		return;
	}

	if (index == vector->numElements - 1) {
		vgeVectorAppend(vector, data);
	}

	uint8_t temp[(vector->numElements - index) * vector->sizeElement];
	memcpy(temp, vector->data + (index * vector->sizeElement), vector->sizeElement);
	vector->numElements++;
	if (vector->numElements > vector->capacity) {
		vector->capacity *= VGE_VECTOR_CAPACITY_GROWTH_FACTOR;
		vector->data = realloc(vector->data, vector->capacity * vector->sizeElement);
	}
	memcpy(vector->data + index * vector->sizeElement, data, vector->sizeElement);
}

void vgeVectorRemoveLast(pVgeVector vector) {
	if (!vector || vector->numElements == 0) {
		return;
	}
	vector->numElements--;
	if (vector->numElements < vector->capacity * VGE_VECTOR_CAPACITY_SHRINK_FACTOR) {
		vector->capacity = vector->capacity * VGE_VECTOR_CAPACITY_SHRINK_FACTOR;
		vector->data = realloc(vector->data, vector->capacity * vector->sizeElement);
	}
}
void vgeVectorRemove(pVgeVector vector, uint64_t index) {
	if (!vector || !vector->data || index >= vector->numElements) {
		return;
	}
	if (index == vector->numElements - 1) {
		vgeVectorRemoveLast(vector);
	}
	memcpy(vector->data + index * vector->sizeElement, vector->data + (index + 1) * vector->sizeElement, vector->sizeElement * (vector->numElements - index));
}
void *vgeVectorGetData(pVgeVector vgeVector) {
	if (!vgeVector) {
		return NULL;
	}
	return vgeVector->data;
}

void *vgeVectorGetElement(pVgeVector vgeVector, uint64_t index) {
	if (!vgeVector || index >= vgeVector->numElements) {
		return NULL;
	}
	return vgeVector->data + index * vgeVector->sizeElement;
}
//
// Created by fabio on 8/27/2025.
//

#include "vgeVector.h"

#include <stdlib.h>
#include <string.h>

pVgeVector vgeVectorInit(uint64_t sizeElement) {
	pVgeVector vgeVector = malloc(sizeof(struct VgeVector));
	if (!vgeVector) {
		return NULL;
	}
	vgeVector->data = malloc(sizeElement * VGE_VECTOR_CAPACITY_INITIAL);
	vgeVector->capacity = VGE_VECTOR_CAPACITY_INITIAL;
	vgeVector->sizeElement = sizeElement;
	vgeVector->numElements = 0;
	return vgeVector;
}

void vgeVectorFree(pVgeVector vgeVector) {
	if (!vgeVector) {
		return;
	}
	free(vgeVector->data);
	free(vgeVector);
}

void *vgeVectorFreeKeepData(pVgeVector vgeVector) {
	if (!vgeVector) {
		return NULL;
	}
	void *data = vgeVector->data;
	free(vgeVector);
	return data;
}

void vgeVectorAppend(pVgeVector vector, void *data) {
	if (!vector || !data) {
		return;
	}
	vector->numElements++;
	if (vector->numElements > vector->capacity) {
		vector->capacity *= VGE_VECTOR_CAPACITY_GROWTH_FACTOR;
		vector->data = realloc(vector->data, vector->capacity * vector->sizeElement);
	}
	memcpy(vector->data + (vector->numElements - 1) * vector->sizeElement, data, vector->sizeElement);
}

void *vgeVectorAppendEmpty(pVgeVector vector) {
	if (!vector) {
		return NULL;
	}
	vector->numElements++;
	if (vector->numElements > vector->capacity) {
		vector->capacity *= VGE_VECTOR_CAPACITY_GROWTH_FACTOR;
		vector->data = realloc(vector->data, vector->capacity * vector->sizeElement);
	}
	memset(vector->data + (vector->numElements - 1) * vector->sizeElement, 0, vector->sizeElement);
	return vector->data + (vector->numElements - 1) * vector->sizeElement;
}

void vgeVectorInsert(pVgeVector vector, void *data, uint64_t index) {
	if (!vector || !data) {
		return;
	}

	if (index >= vector->numElements || !vector->numElements) {
		vgeVectorAppend(vector, data);
		return;
	}

	uint8_t temp[(vector->numElements - index) * vector->sizeElement];
	memcpy(temp, vector->data + (index * vector->sizeElement), vector->sizeElement * (vector->numElements - index));
	vector->numElements++;
	if (vector->numElements > vector->capacity) {
		vector->capacity *= VGE_VECTOR_CAPACITY_GROWTH_FACTOR;
		vector->data = realloc(vector->data, vector->capacity * vector->sizeElement);
	}
	memcpy(vector->data + (index + 1) * vector->sizeElement, temp, vector->sizeElement * (vector->numElements - index));
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
	if (!vector || !vector->data) {
		return;
	}
	if (index >= vector->numElements - 1) {
		vgeVectorRemoveLast(vector);
		return;
	}
	memcpy(vector->data + index * vector->sizeElement, vector->data + (index + 1) * vector->sizeElement, vector->sizeElement * (vector->numElements - index));
}
void *vgeVectorGetData(pVgeVector vgeVector) {
	if (!vgeVector) {
		return NULL;
	}
	return vgeVector->data;
}

uint64_t vgeVectorGetSize(pVgeVector vgeVector) {
	if (!vgeVector) {
		return 0;
	}
	return vgeVector->numElements;
}

uint64_t vgeVectorGetSizeElement(pVgeVector vgeVector) {
	return vgeVector->sizeElement;
}

void *vgeVectorGetElement(pVgeVector vgeVector, uint64_t index) {
	if (!vgeVector || index >= vgeVector->numElements) {
		return NULL;
	}
	return vgeVector->data + index * vgeVector->sizeElement;
}

void *vgeVectorGetLast(pVgeVector vgeVector) {
	if (!vgeVector) {
		return NULL;
	}
	return vgeVectorGetElement(vgeVector, vgeVector->numElements - 1);
}
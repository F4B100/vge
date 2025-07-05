//
// Created by fabio on 6/29/2025.
//

#ifndef MODELSTRUCTS_H
#define MODELSTRUCTS_H
#include <stdint.h>

typedef struct VertexInfo {
	uint32_t offset;
	uint32_t size;
	uint32_t stride;
} vertexInfo;

typedef struct VertexBuffer {
	uint32_t size;
	vertexInfo* info;
	void* data;
} vertexBuffer;

typedef struct IndexBuffer {
	uint32_t size;
	uint32_t* info;
} indexBuffer;

#endif //MODELSTRUCTS_H

//
// Created by fabio on 6/29/2025.
//

#ifndef MODELSTRUCTS_H
#define MODELSTRUCTS_H

typedef struct VertexInfo {
	uint32_t offset;
	uint32_t size;
	uint32_t stride;
}

typedef struct VertexBuffer {
	uint32_t size;
	VertexInfo* info;
	void* data;
}

typedef struct IndexBuffer {
	uint32_t size;
	uint32_t* info;
}

#endif //MODELSTRUCTS_H


#include "src/game/GameMain.h"
#include "src/utils/vector/vgeVector.h"

int main(void) {
	pVgeVector vgeVector = vgeVectorInit(4);
	uint32_t toAdd = 0;
	for (uint64_t i = 0; i < 5; i++) {
		printf("adding %d\n", ++toAdd);
		vgeVectorAppend(vgeVector, &toAdd);
		printf("added %d\n", toAdd);
	}
	for (uint64_t i = 0; i < 5; i++) {
		printf("--------------------------\n");
		printf("%d\n", *(int *)(vgeVectorGetData(vgeVector) + i * sizeof(uint32_t)));
	}
    return 0;
}


//
// Created by fabio on 22-05-2025.
//


#include "vgeMain.h"

#include <stdlib.h>

void startEngine(uint64_t sizeData) {
	void *data = calloc(sizeData, 1);
	GameStart(data);
	while (exitCondition(data)) {
		GameLoop(data);
	}
	GameEnd(data);
}
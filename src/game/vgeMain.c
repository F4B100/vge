//
// Created by fabio on 22-05-2025.
//


#include "vgeMain.h"

#include <stdlib.h>

#include "../utils/vgeThread.h"
#include "../window/vgeWindow.h"

void startEngine(uint64_t sizeData) {
	vgeInit();
	byte data[sizeData];
	memset(data, 0, sizeData);
	GameStart(data);
	while (exitCondition(data)) {
		GameLoop(data);
	}
	GameEnd(data);
}
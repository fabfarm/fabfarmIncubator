#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <SPI.h>
#include <TFT_eSPI.h>

#include "Config.h"
#include "DebugManager.h"
#include "RunIncubator.h"


void initializeTFTDisplay();
void updateTFTDisplay();


#endif
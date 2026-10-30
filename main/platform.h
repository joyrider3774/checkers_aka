// What the game needs from the Gamebuino AKA, on top of the official AKA library: time, the
// keys, the SD card folder and the way back to the launcher.
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>
#include <stdint.h>
#include "gb_common.h"

// the game's folder on the SD card, the one the launcher found firmware.bin in
#define DATA_DIR MOUNT_POINT "/CHECKERS"

void PlatformInit();
uint32_t GetTicks();
void Delay(uint32_t ms);
// waits for the next frame, 60 frames a second
void FramerateDelay();
// the keys held right now, GB_KEY_* bits; the joystick counts as the d-pad. RUN + MENU held for
// half a second goes back to the launcher, RUN on its own switches the AKA off
uint16_t PollKeys();
// a block that does not have to be in internal RAM
void* AllocLarge(size_t size);
// makes the game's folders on the card when they are not there yet
void MakeDataDirs();
// boots the launcher again (the OTA_1 partition), the AKA's way of quitting a game
void ReturnToLoader();

#endif

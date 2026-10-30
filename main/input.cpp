#include "input.h"

#include "gb_common.h"
#include "platform.h"

bool ButLeft = false, ButRight = false, ButUp = false, ButDown = false, ButA = false, ButX = false,
     ButSelect = false, ButVolDown = false, ButVolUp = false;
bool HeldLeft = false, HeldRight = false, HeldUp = false, HeldDown = false;

static uint16_t PreviousKeys = 0;

bool ReadButtons()
{
    const uint16_t Keys = PollKeys();
    const uint16_t Pressed = Keys & ~PreviousKeys;
    PreviousKeys = Keys;
    ButLeft = Pressed & GB_KEY_LEFT;
    ButRight = Pressed & GB_KEY_RIGHT;
    ButUp = Pressed & GB_KEY_UP;
    ButDown = Pressed & GB_KEY_DOWN;
    ButA = Pressed & GB_KEY_A;
    ButX = Pressed & GB_KEY_B;
    ButSelect = Pressed & GB_KEY_MENU;
    ButVolDown = Pressed & GB_KEY_L1;
    ButVolUp = Pressed & GB_KEY_R1;
    HeldLeft = Keys & GB_KEY_LEFT;
    HeldRight = Keys & GB_KEY_RIGHT;
    HeldUp = Keys & GB_KEY_UP;
    HeldDown = Keys & GB_KEY_DOWN;
    return Pressed != 0;
}

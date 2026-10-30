// The GP2X buttons of the game on the AKA's keys.
//
// The game acted on a button when it went down (SDL_JOYBUTTONDOWN). ReadButtons sets one But*
// flag for each key that went down since the last call, keeping the GP2X layout:
//   d-pad / joystick  ButLeft, ButRight, ButUp, ButDown
//   A                 ButA         select, pick up a piece and move it (the GP2X's A)
//   B                 ButX         drop the piece that is picked up, cancel
//   MENU              ButSelect    back (the GP2X's SELECT)
//   L1 / R1           ButVolDown, ButVolUp
// The GP2X had diagonal buttons as well, on the AKA they are two directions held together:
// Held* are the directions held down right now.
#ifndef INPUT_H
#define INPUT_H

extern bool ButLeft, ButRight, ButUp, ButDown, ButA, ButX, ButSelect, ButVolDown, ButVolUp;
extern bool HeldLeft, HeldRight, HeldUp, HeldDown;

// sets the But* flags of the keys that went down since the last call, true when any did
bool ReadButtons();

#endif

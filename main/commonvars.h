// The game's globals, the ones every state of main.cpp shared
#ifndef COMMONVARS_H
#define COMMONVARS_H

#include <vector>
#include "Common.h"

extern GameStates GameState;
extern int Difficulty,Volume;
// the AI also counts how many pieces can jump or move (the Jump option)
extern bool JumpHeuristicEnabled;
// the moves made in the game so far, the panel shows the last ones
extern std::vector<SMove> MadeMoveList;

#endif

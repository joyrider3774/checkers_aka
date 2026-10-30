// What the states have in common: the images, volume, settings and music
#ifndef GAMEFUNCS_H
#define GAMEFUNCS_H

#include <vector>
#include "Common.h"

// the images, from the firmware
void LoadGraphics();
void IncVolume();
void DecVolume();
// the volume, the difficulty and the jump option, in DATA_DIR/settings.dat
void LoadSettings();
void SaveSettings();
// the music plays all the time, over and over (Mix_PlayMusic(Music,-1))
void KeepMusicPlaying();
// the volume buttons every screen had
void VolumeButtons();
// Move is one of List, the same squares by the same player
bool MoveInList(const SMove &Move,const std::vector<SMove> &List);

#endif

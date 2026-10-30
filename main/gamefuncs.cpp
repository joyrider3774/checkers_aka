#include "gamefuncs.h"

#include <stdio.h>

#include "assets.h"
#include "commonvars.h"
#include "gfx.h"
#include "input.h"
#include "platform.h"
#include "sound.h"

// the images, in the order of the Images enum
static const AssetSurface *const Images[NrOfImages] = {
    &ASSET_TITLESCREEN, &ASSET_PLAY1, &ASSET_PLAY2, &ASSET_OPTIONS1, &ASSET_OPTIONS2, &ASSET_QUIT1,
    &ASSET_QUIT2, &ASSET_VERYEASY1, &ASSET_VERYEASY2, &ASSET_EASY1, &ASSET_EASY2, &ASSET_HARD1,
    &ASSET_HARD2, &ASSET_VERYHARD1, &ASSET_VERYHARD2, &ASSET_JUMPENABLED1, &ASSET_JUMPENABLED2,
    &ASSET_JUMPDISABLED1, &ASSET_JUMPDISABLED2, &ASSET_BLACK, &ASSET_WHITE, &ASSET_BLUE, &ASSET_RED,
    &ASSET_REDKING, &ASSET_BLUEKING, &ASSET_PANEL, &ASSET_PLAYER, &ASSET_CPU, &ASSET_PLAYERWINS,
    &ASSET_CPUWINS, &ASSET_DRAW, &ASSET_CONFIRMQUIT};
static_assert(sizeof(ASSET_IMAGE_NAMES) / sizeof(ASSET_IMAGE_NAMES[0]) == NrOfImages,
              "the Images enum does not match tools/convert_assets.py");

void LoadGraphics()
{
    for (int Teller = 0; Teller < NrOfImages; Teller++)
        IMG[Teller] = SurfaceFromAsset(*Images[Teller]);
}

// increase the volume with 8, saved right away: the AKA is switched off, it does not quit
void IncVolume()
{
	if (Volume < 128)
	{
		Volume += 8;
		SetVolume(Volume);
		SaveSettings();
	}
}

void DecVolume()
{
	if (Volume > 0)
	{
		Volume-= 8;
		SetVolume(Volume);
		SaveSettings();
	}
}

// the GP2X file also had the TV out screen position, which the AKA has no use for
void LoadSettings()
{
 	FILE *SettingsFile;
 	int Value=0;
    Volume = 128;
    Difficulty = Hard;
    JumpHeuristicEnabled = false;
 	SettingsFile = fopen(DATA_DIR "/settings.dat","r");
 	if(SettingsFile)
 	{
		if ((fscanf(SettingsFile,"Volume=%d\n",&Volume) != 1) || (Volume < 0) || (Volume > 128))
		    Volume = 128;
        if ((fscanf(SettingsFile,"Difficulty=%d\n",&Difficulty) != 1) || (Difficulty < VeryEasy) || (Difficulty > VeryHard))
            Difficulty = Hard;
        if (fscanf(SettingsFile,"JumpHeuristic=%d\n",&Value) == 1)
            JumpHeuristicEnabled = (Value == 1);
		fclose(SettingsFile);
 	}
    SetVolume(Volume);
}

// Save the settings
void SaveSettings()
{
 	FILE *SettingsFile;
 	SettingsFile = fopen(DATA_DIR "/settings.dat","w");
 	if(SettingsFile)
 	{
		fprintf(SettingsFile,"Volume=%d\n",Volume);
        fprintf(SettingsFile,"Difficulty=%d\n",Difficulty);
        fprintf(SettingsFile,"JumpHeuristic=%d\n",JumpHeuristicEnabled ? 1 : 0);
		fclose(SettingsFile);
 	}
}

void KeepMusicPlaying()
{
    if (!MusicPlaying())
        PlayMusic(0);
}

void VolumeButtons()
{
    if (ButVolUp)
        IncVolume();
    if (ButVolDown)
        DecVolume();
}

bool MoveInList(const SMove &Move,const std::vector<SMove> &List)
{
    for (size_t Teller = 0; Teller < List.size(); Teller++)
        if((Move.NewX == List[Teller].NewX) && (Move.NewY == List[Teller].NewY) &&
           (Move.X == List[Teller].X) && (Move.Y == List[Teller].Y) && (Move.Player == List[Teller].Player))
            return true;
    return false;
}

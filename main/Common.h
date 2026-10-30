#ifndef COMMON_H
#define COMMON_H

#include "gfx.h"

const int
        NrOfRows = 8,
        NrOfCols = 8,
        ComputerPawn = 3,
        HumanPawn = 1,
        ComputerKing = 4,
        HumanKing = 2,
        Human = 1,
        Computer = 2,
        Empty = 0,
        TileWidth = 30,
        TileHeight = 30,
        VeryEasy=1,
        Easy=2,
        Hard=3,
        VeryHard=4,
        NrOfSounds=9,
        SND_SELECT=0,
		SND_TAKEMOVE=1,
		SND_WRONG=2,
		SND_START=3,
		SND_WINNER=4,
		SND_LOSER=5,
		SND_MENU=6,
		SND_MOVE=7,
		SND_DRAW=8; // the Playdate version's

enum GameStates {GSQuit,GSGame,GSTitleScreen,GSOptions};
enum GameResult {CPUWin,PlayerWin,Draw,Playing};

// the images, in the order of ASSET_IMAGE_NAMES (tools/convert_assets.py)
enum Images {
    IMGTitleScreen, IMGPlay1, IMGPlay2, IMGOptions1, IMGOptions2, IMGQuit1, IMGQuit2,
    IMGVeryEasy1, IMGVeryEasy2, IMGEasy1, IMGEasy2, IMGHard1, IMGHard2, IMGVeryHard1, IMGVeryHard2,
    IMGJumpEnabled1, IMGJumpEnabled2, IMGJumpDisabled1, IMGJumpDisabled2,
    IMGBlackSquare, IMGWhiteSquare, IMGBluePawn, IMGRedPawn, IMGRedKing, IMGBlueKing,
    IMGPanel, IMGPlayer, IMGCpu, IMGPlayerWins, IMGCpuWins, IMGDraw, IMGConfirmQuit, NrOfImages
};

// the images, built into the firmware
extern Surface IMG[NrOfImages];

struct SMove
{
    int X,Y,NewX,NewY,DeletedPiece,Player,KingX,KingY;
};

// A struct that holds x & y coordinate
struct SPoint
{
    int X,Y;
};

#endif

#include "COptionMenu.h"
#include "commonvars.h"
#include "gamefuncs.h"
#include "gfx.h"
#include "input.h"
#include "platform.h"
#include "sound.h"
#include "states.h"

// the options screen: up / down choose the line, left / right or A change it, B or MENU go back
// to the title screen. On the GP2X any other button went back, A as well, as the Playdate version
// A changes the option here
void Options()
{
    COptionMenu *Menu = new COptionMenu(Difficulty,JumpHeuristicEnabled);
    while (GameState == GSOptions)
    {
        KeepMusicPlaying();
        ReadButtons();
        VolumeButtons();
        if (ButUp)
            Menu->PreviousItem();
        if (ButDown)
            Menu->NextItem();
        if (ButLeft || ButRight || ButA)
        {
            if (ButLeft)
                Menu->PreviousOption();
            else
                Menu->NextOption();
            Difficulty = Menu->GetOptionDifficulty();
            JumpHeuristicEnabled = Menu->GetOptionJump();
            // the GP2X saved them when it quit, the AKA is just switched off
            SaveSettings();
        }
        if (ButX || ButSelect)
            GameState = GSTitleScreen;
        Menu->Draw(Buffer);
        PresentBuffer(255);
        FramerateDelay();
    }
    delete Menu;
}

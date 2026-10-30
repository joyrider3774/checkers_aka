#include "CMainMenu.h"
#include "commonvars.h"
#include "gamefuncs.h"
#include "gfx.h"
#include "input.h"
#include "platform.h"
#include "sound.h"
#include "states.h"

// main title screen loop. The GP2X quit on SELECT here as well, on the AKA only Quit does; its
// R + d-pad moved the screen for the TV out
void TitleScreen()
{
    CMainMenu *Menu = new CMainMenu();
    while (GameState == GSTitleScreen)
    {
        // the music plays in an endless loop
        KeepMusicPlaying();
        ReadButtons();
        VolumeButtons();
        if (ButUp)
            Menu->PreviousItem();
        if (ButDown)
            Menu->NextItem();
        if (ButA)
        {
            PlaySound(SND_SELECT);
            switch(Menu->GetSelection())
            {
                case 1:
                    GameState = GSGame;
                    break;
                case 2:
                    GameState = GSOptions;
                    break;
                case 3:
                    GameState = GSQuit;
                    break;
            }
        }
        Menu->Draw(Buffer);
        PresentBuffer(255);
        FramerateDelay();
    }
    delete Menu;
}

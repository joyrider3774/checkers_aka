#include "COptionMenu.h"
#include "Common.h"
#include "sound.h"

COptionMenu::COptionMenu(int Difficulty,bool JumpEnabled)
{
    Selection = 1;
    OptionDifficulty = Difficulty;
    OptionJump = JumpEnabled;
}

void COptionMenu::NextOption()
{
    switch(Selection)
    {
        case 1:
            OptionJump = !OptionJump;
            break;
        case 2:
            OptionDifficulty++;
            if (OptionDifficulty > VeryHard)
                OptionDifficulty = VeryEasy;
            break;
    }
    PlaySound(SND_SELECT);
}

void COptionMenu::PreviousOption()
{
    switch(Selection)
    {
        case 1:
            OptionJump = !OptionJump;
            break;
        case 2:
            OptionDifficulty--;
            if (OptionDifficulty < VeryEasy)
                OptionDifficulty = VeryHard;
            break;
    }
    PlaySound(SND_SELECT);
}

void COptionMenu::NextItem()
{
    Selection++;
    if (Selection == 3)
        Selection = 1;
    PlaySound(SND_MENU);
}

void COptionMenu::PreviousItem()
{
    Selection--;
    if (Selection == 0)
        Selection = 2;
    PlaySound(SND_MENU);
}

// Draw the options menu, the selected line purple, the other blue
void COptionMenu::Draw(Surface &Dst)
{
    // draw the title screen background
    BlitSurface(IMG[IMGTitleScreen],NULL,Dst,0,0);
    const bool Sel1 = (Selection == 1);
    if(OptionJump)
        BlitSurface(IMG[Sel1 ? IMGJumpEnabled1 : IMGJumpEnabled2],NULL,Dst,40,62);
    else
        BlitSurface(IMG[Sel1 ? IMGJumpDisabled1 : IMGJumpDisabled2],NULL,Dst,40,62);
    const bool Sel2 = (Selection == 2);
    int Image = IMGHard1;
    switch(OptionDifficulty)
    {
        case VeryEasy:
            Image = Sel2 ? IMGVeryEasy1 : IMGVeryEasy2;
            break;
        case Easy:
            Image = Sel2 ? IMGEasy1 : IMGEasy2;
            break;
        case Hard:
            Image = Sel2 ? IMGHard1 : IMGHard2;
            break;
        case VeryHard:
            Image = Sel2 ? IMGVeryHard1 : IMGVeryHard2;
            break;
    }
    BlitSurface(IMG[Image],NULL,Dst,40,119);
}

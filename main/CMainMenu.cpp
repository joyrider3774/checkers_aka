#include "CMainMenu.h"
#include "Common.h"
#include "sound.h"

// constructor of main menu will set the current selection to 1 (io play)
CMainMenu::CMainMenu()
{
    Selection = 1;
}

// Increase the selection if it goes to far set i to the first selection
void CMainMenu::NextItem()
{
    Selection++;
    if (Selection == 4)
        Selection = 1;
    PlaySound(SND_MENU);
}

// decrease the selection if it goes to low set it to the last selection
void CMainMenu::PreviousItem()
{
    Selection--;
    if (Selection == 0)
        Selection = 3;
    PlaySound(SND_MENU);
}

// Draw the main menu
void CMainMenu::Draw(Surface &Dst)
{
    // draw the title screen background
    BlitSurface(IMG[IMGTitleScreen],NULL,Dst,0,0);
    // the selected entry purple, the others blue
    BlitSurface(IMG[Selection == 1 ? IMGPlay1 : IMGPlay2],NULL,Dst,100,62);
    BlitSurface(IMG[Selection == 2 ? IMGOptions1 : IMGOptions2],NULL,Dst,100,107);
    BlitSurface(IMG[Selection == 3 ? IMGQuit1 : IMGQuit2],NULL,Dst,100,150);
}

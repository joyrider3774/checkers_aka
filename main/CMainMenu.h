#ifndef CMAINMENU_H
#define CMAINMENU_H

#include "gfx.h"

// Class for the main menu
class CMainMenu
{
    private:
        int Selection; //Holds what we currently selected in the main menu, 1 = play, 2= options, 3 = quit
    public:
        CMainMenu(); // constructor
        void Draw(Surface &Dst); //draw to a certain surface
        void NextItem(); //Select the next menuitem
        void PreviousItem(); //select the previous menuitem
        int GetSelection() { return Selection; } // return the current selection
};

#endif

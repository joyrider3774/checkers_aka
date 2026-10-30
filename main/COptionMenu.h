#ifndef COPTIONMENU_H
#define COPTIONMENU_H

#include "gfx.h"

// the options screen: the jump heuristic of the AI and the difficulty
class COptionMenu
{
    private:
        bool OptionJump;
        int Selection,OptionDifficulty;
    public:
        COptionMenu(int Difficulty,bool JumpEnabled); // constructor
        void Draw(Surface &Dst); //draw to a certain surface
        void NextItem(); //Select the next menuitem
        void PreviousItem(); //select the previous menuitem
        void NextOption();
        void PreviousOption();
        int GetOptionDifficulty() {return OptionDifficulty;}
        bool GetOptionJump(){return OptionJump;}
        int GetSelection() { return Selection; } // return the current selection
};

#endif

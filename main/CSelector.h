#ifndef CSELECTOR_H
#define CSELECTOR_H

#include "CBord.h"
#include "Common.h"
#include "gfx.h"

// the human player's cursor on the board, and the piece it picked up
class CSelector
{
    private:
        SPoint SelectedPoint,CurrentPoint; //variables to hold the current position and the selected position

    public:
        bool HasSelection,Visible;
        // constructor, parameters define the starting position
        CSelector(const int PlayFieldXin,const int PlayFieldYin);
        void Hide() { Visible = false; }
        void Show() { Visible = true; }
        // Will set the position only if it layes within the board boundary
        void SetPosition(const int PlayFieldXin,const int PlayFieldYin);
        // Select will set the current position as the selected position
        void Select();
        // return the selected position
        SPoint GetSelection() { return SelectedPoint; }
        // return the current position
        SPoint GetPosition() { return CurrentPoint; }
        void DeSelect() { HasSelection = false; }
        // the cursor, or the piece picked up with the squares it can go to
        void Draw(Surface &Dst,CBord *Bord);
};

#endif

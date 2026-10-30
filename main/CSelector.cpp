#include "CSelector.h"

CSelector::CSelector(const int PlayFieldXin,const int PlayFieldYin)
{
    SelectedPoint.X = -1; // there's no selection set it -1
    SelectedPoint.Y = -1;
    CurrentPoint.X = PlayFieldXin; // set current position
    CurrentPoint.Y = PlayFieldYin;
    HasSelection = false; // No selection when it's created
    Visible = true;
}

void CSelector::SetPosition(const int PlayFieldXin,const int PlayFieldYin)
{
    // check if the new position is inside the board, if so set the new position as the currentposition
    if ((PlayFieldYin>=0) && (PlayFieldYin < NrOfRows) && (PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols))
    {
        CurrentPoint.X = PlayFieldXin;
        CurrentPoint.Y = PlayFieldYin;
    }
}

void CSelector::Select()
{
    SelectedPoint.X = CurrentPoint.X;
    SelectedPoint.Y = CurrentPoint.Y;
    HasSelection = true;
}

// the square X,Y, green as the GP2X cursor was
static void Square(Surface &Dst,int X,int Y,uint8_t r,uint8_t g,uint8_t b)
{
    BoxRGBA(Dst,X * TileWidth,Y * TileHeight,(X+1) * TileWidth - 1,(Y+1) * TileHeight - 1,r,g,b,150);
}

void CSelector::Draw(Surface &Dst,CBord *Bord)
{
    if (!Visible)
        return;
    if(HasSelection)
    {
        // the squares the piece can go to: its jumps, or its steps when it has none
        SMove Move;
        bool JumpFound = false;
        Move.Player = Human;
        Move.KingX = -1;
        Move.KingY = -1;
        Move.DeletedPiece = -1;
        Move.X = SelectedPoint.X;
        Move.Y = SelectedPoint.Y;
        for (int Steps = 2; (Steps >= 1) && !JumpFound; Steps--)
            for (int Dir = 0; Dir < 4; Dir++)
            {
                Move.NewX = SelectedPoint.X + ((Dir == 1) || (Dir == 2) ? Steps : -Steps);
                Move.NewY = SelectedPoint.Y + (Dir < 2 ? -Steps : Steps);
                if(Bord->ValidMove(Move))
                {
                    Square(Dst,Move.NewX,Move.NewY,100,255,50);
                    if (Steps == 2)
                        JumpFound = true;
                }
            }
        // the piece picked up
        Square(Dst,SelectedPoint.X,SelectedPoint.Y,200,100,50);
        // the GP2X picked the square with its diagonal buttons, here the cursor goes to it: a
        // white frame when it is not on the piece
        if ((CurrentPoint.X != SelectedPoint.X) || (CurrentPoint.Y != SelectedPoint.Y))
        {
            RectangleRGBA(Dst,CurrentPoint.X * TileWidth,CurrentPoint.Y * TileHeight,(CurrentPoint.X+1) * TileWidth - 1,(CurrentPoint.Y+1) * TileHeight - 1,255,255,255,255);
            RectangleRGBA(Dst,CurrentPoint.X * TileWidth + 1,CurrentPoint.Y * TileHeight + 1,(CurrentPoint.X+1) * TileWidth - 2,(CurrentPoint.Y+1) * TileHeight - 2,255,255,255,255);
        }
    }
    else
        Square(Dst,CurrentPoint.X,CurrentPoint.Y,100,255,50);
}

#ifndef CBORD_H
#define CBORD_H

#include <vector>
#include "Common.h"
#include "gfx.h"

// the board: 8x8, the pieces on the dark squares, the computer's (red) at the top
class CBord
{
    private:
        int BordArray[8][8];
    public:
        CBord();
        CBord(CBord *Bord);
        void ApplyMove(SMove &move,bool PlaySound);
        // the steps of a move, a jump over several pieces is one step per jump. DelayDraw shows the
        // board half a second after each step but the last
        void ApplyMoves(std::vector<SMove> &Moves,bool reverse,bool DelayDraw,bool PlaySound);
        void UndoMove(SMove &Move);
        void UndoMoves(std::vector<SMove> &Moves,bool reverse);
        void Draw(Surface &Dst);
        bool ValidMove(SMove move);
        int GetBordValue(int X,int Y);
        bool GameEnded();
        bool PieceCanJump(int X,int Y);
        bool PieceCanMove(int X,int Y);
        // the jumps Player can make, of all its pieces or of the piece on X,Y
        std::vector<SMove> GetSingleTakeMoves(int Player);
        std::vector<SMove> GetSingleTakeMoves(int Player,int X,int Y);
        // every whole jump (one or more jumps in a row) of Player, the steps of each from the last to the first
        std::vector<std::vector<SMove> > GetTakeMoves(int Player);
        std::vector<std::vector<SMove> > GenerateTakeMoves(int Player,int X,int Y);
        std::vector<std::vector<SMove> > GetNormalMoves(int Player);
};

#endif

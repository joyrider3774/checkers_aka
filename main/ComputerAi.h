#ifndef COMPUTERAI_H
#define COMPUTERAI_H

#include <vector>
#include "CBord.h"

// the computer player: a min max search with alpha beta pruning, as deep as the difficulty.
// The GP2X game had three more of these with other ways to value a board, which it never used
class ComputerAi
{
    protected:
        int SearchDepth;
        bool JumpHeuristicEnabled;
    private:
        int Max(CBord *Bord,int depth,int alpha,int beta);
        int Min(CBord *Bord,int depth,int alpha,int beta);
        std::vector<SMove> MinMax(CBord *Bord,int depth,int Player);
        int EvaluateBord(CBord *Bord,int Player);
        std::vector<std::vector<SMove> > GenerateMovesList(CBord *Bord,int Player);
    public:
        ComputerAi(int SearchDepthIn,bool JumpHeuristicEnabledIn);
        // makes the best move of Player on the board, false when it has none
        bool MakeMove(CBord *Bord,int Player);
};

#endif

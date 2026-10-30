#include "ComputerAi.h"

#include <limits.h>

#include "commonvars.h"
#include "platform.h"

using std::vector;

ComputerAi::ComputerAi(int SearchDepthIn,bool JumpHeuristicEnabledIn)
{
    SearchDepth=SearchDepthIn;
    JumpHeuristicEnabled = JumpHeuristicEnabledIn;
}

// a jump has to be made when there is one
vector<vector<SMove> > ComputerAi::GenerateMovesList(CBord *Bord,int Player)
{
    vector<vector<SMove> > MoveList = Bord->GetTakeMoves(Player);
    if(MoveList.size() > 0)
        return MoveList;
    else
        return Bord->GetNormalMoves(Player);
}

// how good the board is for the computer: its pieces against the human's, pawns that went far
// and kings in the middle, and with the jump option how many pieces can jump or move
int ComputerAi::EvaluateBord(CBord *Bord,int Player)
{
    int NumHumanJumps=0,NumHumanMoves=0,NumComputerJumps=0,NumComputerMoves=0,Value=0,X,Y,NumComputerPawns=0,NumHumanPawns=0,NumComputerKings=0,NumHumanKings=0;
    for (Y=0;Y<NrOfRows;Y++)
        for (X=0;X<NrOfCols;X++)
        {
            if ((Y*(NrOfRows-1) + X)%2 == 1)
            {
                if(Bord->GetBordValue(X,Y) == HumanPawn)
                {
                    NumHumanPawns++;
                    if(JumpHeuristicEnabled)
                    {
                        if(Bord->PieceCanJump(X,Y))
                            NumHumanJumps++;
                        else
                            if(Bord->PieceCanMove(X,Y))
                                NumHumanMoves++;
                    }
                    Value -= 7-Y;
                }
                if (Bord->GetBordValue(X,Y) == HumanKing)
                {
                    NumHumanKings++;
                    if(JumpHeuristicEnabled)
                    {
                        if(Bord->PieceCanJump(X,Y))
                            NumHumanJumps++;
                        else
                            if(Bord->PieceCanMove(X,Y))
                                NumHumanMoves++;
                    }
                    if((Y >= 3) && (Y <= 4))
                    {
                        if((X==0) || (X==1) || (X==6) || (X==7))
                            Value -= 3;
                        else
                            Value -=4;
                    }
                    else
                        if(Y <3)
                            Value -= Y;
                        else
                            Value -= (7-Y);
                }
                if(Bord->GetBordValue(X,Y) == ComputerPawn)
                {
                    NumComputerPawns++;
                    if(JumpHeuristicEnabled)
                    {
                        if(Bord->PieceCanJump(X,Y))
                            NumComputerJumps++;
                        else
                            if(Bord->PieceCanMove(X,Y))
                                NumComputerMoves++;
                    }
                    Value += Y;
                }
                if (Bord->GetBordValue(X,Y) == ComputerKing)
                {
                    NumComputerKings++;
                    if(JumpHeuristicEnabled)
                    {
                        if(Bord->PieceCanJump(X,Y))
                            NumComputerJumps++;
                        else
                            if(Bord->PieceCanMove(X,Y))
                                NumComputerMoves++;
                    }
                    if((Y >= 3) && (Y <= 4))
                    {
                        if((X==0) || (X==1) || (X==6) || (X==7))
                            Value += 3;
                        else
                            Value +=4;
                    }
                    else
                        if(Y <3)
                            Value += Y;
                        else
                            Value += (7-Y);
                }
            }
        }
    Value += ((NumComputerPawns - NumHumanPawns)*40) +((NumComputerKings - NumHumanKings)*80);
    if (JumpHeuristicEnabled)
    {
        if (NumHumanJumps > 0)
            Value -= NumHumanJumps*3;
        else
            Value -= NumHumanMoves;
        if (NumComputerJumps > 0)
            Value += NumComputerJumps*3;
        else
            Value += NumComputerMoves;
    }
    return  Value;
}

int ComputerAi::Max(CBord *Bord,int depth, int alpha, int beta)
{
    int Value;
    if ((depth <= 0) || Bord->GameEnded())
        return EvaluateBord(Bord,Computer);
    vector<vector<SMove> > Moves = GenerateMovesList(Bord,Computer);
    if (Moves.size()  == 0)
        return EvaluateBord(Bord,Computer);
    for (vector<vector<SMove> >::iterator Iter=Moves.begin(); Iter != Moves.end(); Iter++)
    {
        Bord->ApplyMoves(*Iter,true,false,false);
        Value = Min(Bord,depth-1,alpha,beta);
        Bord->UndoMoves(*Iter,false);
        if (Value > alpha)
            alpha = Value;
        if (alpha > beta)
            return beta;
    }
    return alpha;
}

int ComputerAi::Min(CBord *Bord,int depth, int alpha, int beta)
{
    int Value;
    if ((depth <= 0) || Bord->GameEnded() )
        return EvaluateBord(Bord,Human);
    vector<vector<SMove> > Moves = GenerateMovesList(Bord,Human);
    if (Moves.size()  == 0)
        return EvaluateBord(Bord,Human);
    for (vector<vector<SMove> >::iterator Iter=Moves.begin(); Iter != Moves.end(); Iter++)
    {
        Bord->ApplyMoves(*Iter,true,false,false);
        Value = Max(Bord,depth-1,alpha,beta);
        Bord->UndoMoves(*Iter,false);
        if (Value < beta)
            beta = Value;
        if (beta < alpha)
            return alpha;
    }
    return beta;
}

vector<SMove> ComputerAi::MinMax(CBord *Bord,int depth,int Player)
{
    int BestValue,Value;
    vector<SMove> BestMove;
    vector<vector<SMove> > Moves = GenerateMovesList(Bord,Player);
    if(Player == Computer)
        BestValue = INT_MIN;
    else
        BestValue = INT_MAX;
    for (vector<vector<SMove> >::iterator iter = Moves.begin(); iter != Moves.end(); iter++)
    {
        Bord->ApplyMoves(*iter,true,false,false);
        if (Player==Computer)
        {
            Value = Min(Bord,depth,INT_MIN,INT_MAX);
            Bord->UndoMoves(*iter,false);
            if(Value > BestValue)
            {
                BestMove = *iter;
                BestValue = Value;
            }
        }
        else
        {
            Value = Max(Bord,depth,INT_MIN,INT_MAX);
            Bord->UndoMoves(*iter,false);
            if(Value < BestValue)
            {
                BestMove = *iter;
                BestValue = Value;
            }
        }
    }
    return BestMove;
}

bool  ComputerAi::MakeMove(CBord *Bord,int Player)
{
    uint32_t Start= GetTicks(),End=0;
    vector<SMove> ComputerMove = MinMax(Bord,SearchDepth,Player);
    End = GetTicks();
    // the computer takes at least 350 ms to think
    if(End-Start < 350)
        Delay(350 - (End-Start));
    Bord->ApplyMoves(ComputerMove,true,true,true);
    // the steps are from the last to the first, the GP2X game listed them that way round
    for (vector<SMove>::reverse_iterator riter = ComputerMove.rbegin(); riter != ComputerMove.rend(); riter++)
        MadeMoveList.push_back(*riter);
    return (ComputerMove.size() > 0);
}

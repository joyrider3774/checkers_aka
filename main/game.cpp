#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "CBord.h"
#include "CSelector.h"
#include "ComputerAi.h"
#include "commonvars.h"
#include "gamefuncs.h"
#include "gfx.h"
#include "input.h"
#include "platform.h"
#include "sound.h"
#include "states.h"

using std::vector;

// the human player's turn: what is needed between frames
struct SHumanTurn
{
    CBord *Bord;
    CSelector *Selector;
    int PlayerNr;
    // the piece jumped and can jump again, it has to
    bool HumanAnotherMove;
    vector<SMove> TakeMovesList,NextTakeMoveList;
};

// the turn goes to the computer
static void EndHumanTurn(SHumanTurn &Turn)
{
    Turn.PlayerNr = Computer;
    Turn.HumanAnotherMove = false;
    Turn.Selector->HasSelection = false;
    Turn.Selector->Hide();
}

// after a jump: when the piece was not crowned and can jump on, it has to, otherwise the turn ends
static void AfterJump(SHumanTurn &Turn,SMove &Move)
{
    if((Move.KingX == -1) && (Move.KingY == -1))
    {
        Turn.NextTakeMoveList = Turn.Bord->GetSingleTakeMoves(Human,Move.NewX,Move.NewY);
        if(Turn.NextTakeMoveList.size() > 0)
        {
            Turn.HumanAnotherMove = true;
            Turn.Selector->Select();
            return;
        }
    }
    EndHumanTurn(Turn);
}

// The picked up piece goes Xi,Yi (1 or 2 squares along a diagonal), what the GP2X's diagonal
// buttons did. A jump has to be made when there is one, and a piece that jumped and can jump
// again has to do that
static void HumanMove(SHumanTurn &Turn,int Xi,int Yi)
{
    CBord *Bord = Turn.Bord;
    CSelector *Selector = Turn.Selector;
    SMove Move;
    Move.Player = Turn.PlayerNr;
    Move.DeletedPiece = -1;
    Move.KingX = -1;
    Move.KingY = -1;
    Move.X = Selector->GetSelection().X;
    Move.Y = Selector->GetSelection().Y;
    Move.NewX = Move.X + Xi;
    Move.NewY = Move.Y + Yi;
    if ((abs(Xi) == 2) && Bord->ValidMove(Move))
    {
        if(!Turn.HumanAnotherMove)
        {
            Turn.TakeMovesList = Bord->GetSingleTakeMoves(Human);
            if(MoveInList(Move,Turn.TakeMovesList))
            {
                Bord->ApplyMove(Move,true);
                MadeMoveList.push_back(Move);
                Selector->SetPosition(Move.NewX,Move.NewY);
                AfterJump(Turn,Move);
            }
            else
                PlaySound(SND_WRONG);
        }
        else
        {
            if(MoveInList(Move,Turn.NextTakeMoveList))
            {
                Bord->ApplyMove(Move,true);
                MadeMoveList.push_back(Move);
                Selector->SetPosition(Move.NewX,Move.NewY);
                Selector->HasSelection = false;
                AfterJump(Turn,Move);
            }
            else
                PlaySound(SND_WRONG);
        }
    }
    else
    {
        // a step, when there is no jump to make
        Turn.TakeMovesList = Bord->GetSingleTakeMoves(Human);
        if ((abs(Xi) == 1) && (Turn.TakeMovesList.size() == 0) && Bord->ValidMove(Move))
        {
            Bord->ApplyMove(Move,true);
            MadeMoveList.push_back(Move);
            Selector->SetPosition(Move.NewX,Move.NewY);
            EndHumanTurn(Turn);
        }
        else
            PlaySound(SND_WRONG);
    }
}

// a diagonal held, the GP2X's diagonal buttons: the picked up piece jumps that way when it can,
// otherwise it steps
static void HumanDiagonal(SHumanTurn &Turn,int Xi,int Yi)
{
    SMove Move;
    Move.Player = Human;
    Move.DeletedPiece = -1;
    Move.KingX = -1;
    Move.KingY = -1;
    Move.X = Turn.Selector->GetSelection().X;
    Move.Y = Turn.Selector->GetSelection().Y;
    Move.NewX = Move.X + Xi*2;
    Move.NewY = Move.Y + Yi*2;
    if (Turn.Bord->ValidMove(Move))
        HumanMove(Turn,Xi*2,Yi*2);
    else
        HumanMove(Turn,Xi,Yi);
}

// the board, the panel with whose turn it is and the last moves, and the result
static void DrawGame(CBord *Bord,CSelector *Selector,int PlayerNr,GameResult Result,bool ConfirmingQuit)
{
    const Color Color1 = {255,160,160},Color2={160,160,255};
    char Msg[300];
    int Teller,StartFor;
    Bord->Draw(Buffer);
    if (!Bord->GameEnded())
        Selector->Draw(Buffer,Bord);
    BlitSurface(IMG[IMGPanel],NULL,Buffer,240,0);
    BlitSurface(IMG[PlayerNr == Human ? IMGPlayer : IMGCpu],NULL,Buffer,245,5);
    if(Result == Draw)
        BlitSurface(IMG[IMGDraw],NULL,Buffer,4,76);
    if(Result == PlayerWin)
        BlitSurface(IMG[IMGPlayerWins],NULL,Buffer,4,76);
    if(Result == CPUWin)
        BlitSurface(IMG[IMGCpuWins],NULL,Buffer,4,76);
    if(ConfirmingQuit)
        BlitSurface(IMG[IMGConfirmQuit],NULL,Buffer,4,76);
    // the last 11 moves, the human's blue, the computer's red
    StartFor = (int)MadeMoveList.size()-11;
    if(StartFor < 0)
        StartFor = 0;
    for(Teller=StartFor;Teller<(int)MadeMoveList.size();Teller++)
    {
        snprintf(Msg,sizeof(Msg),"(%d,%d)>(%d,%d)",MadeMoveList[Teller].X+1,MadeMoveList[Teller].Y+1,MadeMoveList[Teller].NewX+1,MadeMoveList[Teller].NewY+1);
        WriteText(Buffer,font,Msg,247,75+((Teller-StartFor)*14),0,MadeMoveList[Teller].Player == Human ? Color2 : Color1);
    }
}

// the game: the human (blue, at the bottom) against the computer (red)
void Game()
{
    GameResult Result = Playing;
    int NumHumanPieces=0,NumComputerPieces=0,X,Y;
    ComputerAi *Ai = new ComputerAi(Difficulty,JumpHeuristicEnabled);
    SHumanTurn Turn;
    Turn.Bord = new CBord();
    Turn.Selector = new CSelector(4,3);
    Turn.PlayerNr = Human;
    Turn.HumanAnotherMove = false;
    CBord *Bord = Turn.Bord;
    CSelector *Selector = Turn.Selector;
    // the Playdate version asks before a game is left, the GP2X left it on SELECT
    bool ConfirmingQuit = false;
    // a diagonal made a move, the next one needs the directions to be let go first
    bool DiagonalDone = false;
    // the GP2X game kept the moves of the game before
    MadeMoveList.clear();
    PlaySound(SND_START);
    while (GameState == GSGame)
    {
        KeepMusicPlaying();
        ReadButtons();
        VolumeButtons();
        if (ConfirmingQuit)
        {
            if (ButA)
                GameState = GSTitleScreen;
            if (ButX || ButSelect)
                ConfirmingQuit = false;
        }
        else if (ButSelect)
        {
            if (Result == Playing)
                ConfirmingQuit = true;
            else
                GameState = GSTitleScreen;
        }
        else if (Result == Playing)
        {
            // the cursor, also to the square a picked up piece goes to
            if (ButLeft)
                Selector->SetPosition(Selector->GetPosition().X-1,Selector->GetPosition().Y);
            if (ButRight)
                Selector->SetPosition(Selector->GetPosition().X+1,Selector->GetPosition().Y);
            if (ButUp)
                Selector->SetPosition(Selector->GetPosition().X,Selector->GetPosition().Y-1);
            if (ButDown)
                Selector->SetPosition(Selector->GetPosition().X,Selector->GetPosition().Y+1);
            const bool Diagonal = (HeldLeft || HeldRight) && (HeldUp || HeldDown);
            if (!Diagonal)
                DiagonalDone = false;
            else if (!DiagonalDone && Selector->HasSelection && (Turn.PlayerNr == Human))
            {
                DiagonalDone = true;
                HumanDiagonal(Turn,HeldLeft ? -1 : 1,HeldUp ? -1 : 1);
            }
            if (ButX && Selector->HasSelection && !Turn.HumanAnotherMove)
            {
                // drop the piece, back on it
                Selector->DeSelect();
                Selector->SetPosition(Selector->GetSelection().X,Selector->GetSelection().Y);
            }
            if (ButA && (Turn.PlayerNr == Human))
            {
                const int Xi = Selector->GetPosition().X - Selector->GetSelection().X;
                const int Yi = Selector->GetPosition().Y - Selector->GetSelection().Y;
                if (!Selector->HasSelection)
                {
                    const int Value = Bord->GetBordValue(Selector->GetPosition().X,Selector->GetPosition().Y);
                    if ((Value == HumanPawn) || (Value == HumanKing))
                    {
                        PlaySound(SND_SELECT);
                        Selector->Select();
                    }
                }
                else if ((Xi == 0) && (Yi == 0))
                {
                    // A on the piece itself drops it, not while it has to jump on
                    if (!Turn.HumanAnotherMove)
                        Selector->DeSelect();
                }
                else if ((abs(Xi) == abs(Yi)) && (abs(Xi) <= 2))
                    HumanMove(Turn,Xi,Yi);
                else
                    PlaySound(SND_WRONG);
            }
        }
        else if (ButA)
            GameState = GSTitleScreen;

        DrawGame(Bord,Selector,Turn.PlayerNr,Result,ConfirmingQuit);
        PresentBuffer(255);
        FramerateDelay();

        if(ConfirmingQuit)
            continue;
        // the GP2X game went on asking the computer for a move when it had none, every frame,
        // and played the winner sound each time: only while the game is on now
        if((Result == Playing) && (Turn.PlayerNr==Computer) && !Bord->GameEnded())
        {
            if(Ai->MakeMove(Bord,Computer))
            {
                Turn.PlayerNr = Human;
                Selector->Show();
            }
            else
            {
                Result = PlayerWin;
                PlaySound(SND_WINNER);
            }
        }
        if((Result==Playing) && (Turn.PlayerNr == Human) && (Bord->GetTakeMoves(Human).size() == 0) && (Bord->GetNormalMoves(Human).size() == 0))
        {
            Selector->Hide();
            Result = CPUWin;
            PlaySound(SND_LOSER);
        }
        if((Result == Playing) && Bord->GameEnded())
        {
            for(Y=0;Y<NrOfRows;Y++)
                for(X=0;X<NrOfCols;X++)
                    if(Bord->GetBordValue(X,Y) != Empty)
                    {
                        if((Bord->GetBordValue(X,Y) == HumanKing) || (Bord->GetBordValue(X,Y) == HumanPawn))
                            NumHumanPieces++;
                        else
                            NumComputerPieces++;
                    }
            if((NumHumanPieces == 1) && (NumComputerPieces == 1))
            {
                Result = Draw;
                PlaySound(SND_DRAW);
            }
            else
                if(NumHumanPieces > 0)
                {
                    Result = PlayerWin;
                    PlaySound(SND_WINNER);
                }
                else
                    if(NumComputerPieces > 0)
                    {
                        Result = CPUWin;
                        PlaySound(SND_LOSER);
                    }
        }
    }
    delete Selector;
    delete Ai;
    delete Bord;
    HaltSounds();
}

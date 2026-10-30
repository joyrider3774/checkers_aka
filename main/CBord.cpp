#include "CBord.h"

#include <stdlib.h>

#include "platform.h"
#include "sound.h"

using std::vector;

// the four diagonals, in the order the GP2X game tried them in
static const int DirX[4] = {-1, 1, 1, -1}, DirY[4] = {1, 1, -1, -1};

// a dark square, the ones the pieces stand on
static inline bool DarkSquare(int X,int Y)
{
    return ((Y*(NrOfRows-1) + X) & 1) != 0;
}

CBord::CBord()
{
    int X,Y;
    for (Y=0;Y<NrOfRows;Y++)
        for (X=0;X<NrOfCols;X++)
            if(DarkSquare(X,Y))
            {
                if (Y<3)
                    BordArray[X][Y] = ComputerPawn;
                else
                    if(Y>4)
                        BordArray[X][Y]=HumanPawn;
                    else
                        BordArray[X][Y] = Empty;
            }
            else
                BordArray[X][Y] = 0;
}

CBord::CBord( CBord *Bord)
{
    int X,Y;
    for (Y=0;Y<NrOfRows;Y++)
        for (X=0;X<NrOfCols;X++)
            BordArray[X][Y] = Bord->GetBordValue(X,Y);
}

// the piece on X,Y can jump, or with Steps 1 move, in any direction
static bool PieceCanGo(CBord *Bord,int X,int Y,int Steps)
{
    int Value = Bord->GetBordValue(X,Y);
    if( Value == Empty)
        return false;
    SMove Move;
    if((Value == HumanKing) || (Value==HumanPawn))
        Move.Player = Human;
    else
       Move.Player = Computer;
    Move.DeletedPiece = -1;
    Move.KingX = -1;
    Move.KingY = -1;
    Move.X = X;
    Move.Y = Y;
    for (int Dir = 0; Dir < 4; Dir++)
    {
        Move.NewX = X + DirX[Dir]*Steps;
        Move.NewY = Y + DirY[Dir]*Steps;
        if (Bord->ValidMove(Move))
            return true;
    }
    return false;
}

bool CBord::PieceCanJump(int X,int Y)
{
    return PieceCanGo(this,X,Y,2);
}

bool CBord::PieceCanMove(int X,int Y)
{
    return PieceCanGo(this,X,Y,1);
}

void CBord::ApplyMove(SMove &move,bool PlaySnd)
{
    int Xi=0,Yi=0;
    if((move.Player == Computer) && (move.NewY == NrOfRows -1) && (BordArray[move.X][move.Y] == ComputerPawn) &&(move.KingY == -1) && (move.KingX == -1) )
    {
        BordArray[move.NewX][move.NewY] = ComputerKing;
        move.KingY = move.NewY;
        move.KingX = move.NewX;
    }
    else
        if((move.Player == Human) && (move.NewY == 0) && (BordArray[move.X][move.Y] == HumanPawn) && (move.KingY == -1) && (move.KingX == -1))
        {
            BordArray[move.NewX][move.NewY] = HumanKing;
            move.KingY = move.NewY;
            move.KingX = move.NewX;
        }
        else
            BordArray[move.NewX][move.NewY] = BordArray[move.X][move.Y];
    BordArray[move.X][move.Y] = 0;
    Xi = move.NewX - move.X;
    Yi = move.NewY - move.Y;
    if ((abs(Yi) == 2) && (abs(Xi) == 2))
    {
        move.DeletedPiece = BordArray[move.NewX - Xi/2][move.NewY - Yi/2];
        BordArray[move.NewX - Xi/2][move.NewY - Yi/2] = 0;
        if (PlaySnd)
            PlaySound(SND_TAKEMOVE);
    }
    else
    {
        if (PlaySnd)
            PlaySound(SND_MOVE);
    }
}

// the board after a step, shown for half a second (the computer's jumps one by one)
static void ShowStep(CBord *Bord)
{
    Bord->Draw(Buffer);
    PresentBuffer(255);
    Delay(500);
}

void CBord::ApplyMoves(vector<SMove> &Moves,bool reverse,bool DelayDraw,bool PlaySnd)
{
    if (reverse)
    {
        for (vector<SMove>::reverse_iterator riter = Moves.rbegin(); riter != Moves.rend(); )
        {
            ApplyMove(*riter,PlaySnd);
            riter++;
            if(DelayDraw && (riter != Moves.rend()))
                ShowStep(this);
        }
    }
    else
    {
        // the GP2X game tested the reverse iterator here, which was not set
        for (vector<SMove>::iterator iter = Moves.begin(); iter != Moves.end(); )
        {
            ApplyMove(*iter,PlaySnd);
            iter++;
            if(DelayDraw && (iter != Moves.end()))
                ShowStep(this);
        }
    }
}

void CBord::UndoMove(SMove &Move)
{
    int Xi=0,Yi=0;
    if((Move.Player == Computer) && (Move.KingY == Move.NewY) && (Move.KingX == Move.NewX))
    {
        BordArray[Move.X][Move.Y] = ComputerPawn;
        Move.KingX = -1;
        Move.KingY = -1;
    }
    else
        if((Move.Player == Human) && (Move.KingY == Move.NewY) && (Move.KingX == Move.NewX))
        {
            BordArray[Move.X][Move.Y] = HumanPawn;
            Move.KingX = -1;
            Move.KingY = -1;
        }
        else
            BordArray[Move.X][Move.Y] = BordArray[Move.NewX][Move.NewY];
    BordArray[Move.NewX][Move.NewY] = 0;
    Xi = Move.NewX - Move.X;
    Yi = Move.NewY - Move.Y;
    if ((abs(Yi) == 2) && (abs(Xi) == 2))
        BordArray[Move.NewX - Xi/2][Move.NewY - Yi/2] = Move.DeletedPiece;
}

void CBord::UndoMoves(vector<SMove> &Moves,bool reverse)
{
    if (reverse)
    {
        for (vector<SMove>::reverse_iterator riter = Moves.rbegin(); riter != Moves.rend(); riter++)
            UndoMove(*riter);
    }
    else
    {
        for (vector<SMove>::iterator iter = Moves.begin(); iter != Moves.end(); iter++)
            UndoMove(*iter);
    }
}

void CBord::Draw(Surface &Dst)
{
    int X,Y;
    for (Y=0;Y<NrOfRows;Y++)
    {
        for (X=0;X<NrOfCols;X++)
        {
            int Image;
            switch(BordArray[X][Y])
            {
                case HumanPawn:
                    Image = IMGBluePawn;
                    break;
                case ComputerPawn:
                    Image = IMGRedPawn;
                    break;
                case ComputerKing:
                    Image = IMGRedKing;
                    break;
                case HumanKing:
                    Image = IMGBlueKing;
                    break;
                default:
                    Image = DarkSquare(X,Y) ? IMGBlackSquare : IMGWhiteSquare;
                    break;
            }
            BlitSurface(IMG[Image],NULL,Dst,X * TileWidth,Y * TileHeight);
        }
    }
}

int CBord::GetBordValue(int X,int Y)
{
    return BordArray[X][Y];
}

bool CBord::GameEnded()
{
    int X,Y,NumHumanPieces=0,NumComputerPieces=0;
    for (Y=0;Y<NrOfRows;Y++)
        for (X=0;X<NrOfCols;X++)
        {
            if((BordArray[X][Y] == HumanPawn) || (BordArray[X][Y] == HumanKing))
                NumHumanPieces++;
            if((BordArray[X][Y] == ComputerPawn) || (BordArray[X][Y] == ComputerKing))
                NumComputerPieces++;
        }
    return ((NumComputerPieces ==0) || (NumHumanPieces == 0) || ((NumHumanPieces == 1) && (NumComputerPieces == 1) && (GetSingleTakeMoves(Human).size() == 0) && (GetSingleTakeMoves(Computer).size() == 0)));
}

vector<SMove> CBord::GetSingleTakeMoves(int Player,int X,int Y)
{
    SMove Move;
    vector<SMove> TakeMoveList;
    Move.Player = Player;
    Move.DeletedPiece = -1;
    Move.KingX = -1;
    Move.KingY = -1;
    Move.X = X;
    Move.Y = Y;
    for (int Dir = 0; Dir < 4; Dir++)
    {
        Move.NewX = X + DirX[Dir]*2;
        Move.NewY = Y + DirY[Dir]*2;
        if (ValidMove(Move))
            TakeMoveList.push_back(Move);
    }
    return TakeMoveList;
}

// a piece of Player is on X,Y
static inline bool PlayersPiece(int Value,int Player)
{
    return ((Player==Human) && ((Value == HumanKing) || (Value == HumanPawn))) ||
           ((Player==Computer) && ((Value == ComputerKing) || (Value == ComputerPawn)));
}

vector<SMove> CBord::GetSingleTakeMoves(int Player)
{
    int X,Y;
    vector<SMove> TakeMoveList,PieceMoves;
    for (Y=0;Y<NrOfRows;Y++)
        for (X=0;X<NrOfCols;X++)
            if(PlayersPiece(BordArray[X][Y],Player))
            {
                PieceMoves = GetSingleTakeMoves(Player,X,Y);
                TakeMoveList.insert(TakeMoveList.end(),PieceMoves.begin(),PieceMoves.end());
            }
    return TakeMoveList;
}

vector<vector<SMove> > CBord::GetNormalMoves(int Player)
{
    int X,Y;
    SMove Move;
    vector<vector<SMove> > MoveList;
    Move.Player = Player;
    Move.DeletedPiece = -1;
    Move.KingX = -1;
    Move.KingY = -1;
    for (Y=0;Y<NrOfRows;Y++)
        for (X=0;X<NrOfCols;X++)
            if(PlayersPiece(BordArray[X][Y],Player))
            {
                Move.X = X;
                Move.Y = Y;
                for (int Dir = 0; Dir < 4; Dir++)
                {
                    Move.NewX = X + DirX[Dir];
                    Move.NewY = Y + DirY[Dir];
                    if (ValidMove(Move))
                        MoveList.push_back(vector<SMove>(1,Move));
                }
            }
    return MoveList;
}

vector<vector<SMove> > CBord::GetTakeMoves(int Player)
{
    int X,Y;
    vector<vector<SMove> > TakeMoveList,TakeMoveList1;
    for (Y=0;Y<NrOfRows;Y++)
        for (X=0;X<NrOfCols;X++)
            if(PlayersPiece(BordArray[X][Y],Player))
            {
                TakeMoveList1 = GenerateTakeMoves(Player,X,Y);
                TakeMoveList.insert(TakeMoveList.end(),TakeMoveList1.begin(),TakeMoveList1.end());
            }
    return TakeMoveList;
}

vector<vector<SMove> > CBord::GenerateTakeMoves(int Player,int X,int Y)
{
    SMove Move;
    int NumMoves=0;
    vector<SMove> Moves,Moves1;
    vector<vector<SMove> > MoveList,MoveList1;
    int teller1,teller2,MoveListIntialSize;
    bool found;
    //check for must take moves
    Move.Player = Player;
    Move.X = X;
    Move.Y = Y;
    Move.DeletedPiece = -1;
    Move.KingX = -1;
    Move.KingY = -1;
    for (int Dir = 0; Dir < 4; Dir++)
    {
        Move.NewX = X + DirX[Dir]*2;
        Move.NewY = Y + DirY[Dir]*2;
        if (ValidMove(Move))
        {
            Moves.push_back(Move);
            ApplyMove(Move,false);
            // when the piece was not crowned by the jump, it can jump on from where it landed
            if((Move.KingX == -1) && (Move.KingY==-1))
            {
                MoveList1 = GenerateTakeMoves(Player,Move.NewX,Move.NewY);
                // the jumps found from there go in the move list
                MoveList.insert(MoveList.end(),MoveList1.begin(),MoveList1.end());
            }
            UndoMove(Move);
            NumMoves++;
        }
    }

    // when there were jumps
    if(NumMoves > 0)
    {
        // and none went on, each jump is a move of its own
        if(MoveList.size() == 0)
        {
            for (size_t Teller = 0; Teller < Moves.size(); Teller++)
            {
                Moves1.clear();
                Moves1.push_back(Moves[Teller]);
                MoveList.push_back(Moves1);
            }
        }
        // otherwise each jump goes after the jumps that went on from where it lands (the steps of
        // a move are from the last to the first)
        else
        {
            MoveListIntialSize = MoveList.size();
            for(teller1=0;teller1<(int)Moves.size();teller1++)
            {
                found = false;
                for(teller2=0;teller2<MoveListIntialSize;teller2++)
                {
                    if(((MoveList[teller2].back()).X == Moves[teller1].NewX) && ((MoveList[teller2].back()).Y == Moves[teller1].NewY))
                    {
                        MoveList[teller2].push_back(Moves[teller1]);
                        found = true;
                    }
                }
                // a jump that no jump went on from is a move of its own
                if (!found)
                {
                    Moves1.clear();
                    Moves1.push_back(Moves[teller1]);
                    MoveList.push_back(Moves1);
                }
            }
        }
    }
    return MoveList;
}

bool  CBord::ValidMove(SMove move)
{
    int Xi=0,Yi=0,Player=move.Player;
    if ((move.NewX < 0) || (move.NewX >= NrOfCols) || (move.NewY < 0) || (move.NewY >= NrOfRows))
        return false;
    // both on a dark square, the new one empty and the piece there
    if (!DarkSquare(move.X,move.Y) || !DarkSquare(move.NewX,move.NewY))
        return false;
    if ((BordArray[move.NewX][move.NewY] != 0) || (BordArray[move.X][move.Y] == Empty))
        return false;
    Xi = move.NewX - move.X;
    Yi = move.NewY - move.Y;
    const int Piece = BordArray[move.X][move.Y];
    if((Piece == HumanPawn) || (Piece == HumanKing))
    {
        if (Player != Human)
            return false;
        const int Over = ((abs(Xi) == 2) && (abs(Yi) == 2)) ? BordArray[move.NewX - Xi/2][move.NewY - Yi/2] : Empty;
        // forwards a step, or a jump over a computer piece
        if ((Yi == -1) && (abs(Xi) == 1))
            return true;
        if ((Yi == -2) && (abs(Xi) == 2))
            return (Over == ComputerPawn) || (Over == ComputerKing);
        // backwards only a king
        if ((Yi == 1) && (abs(Xi) == 1))
            return (Piece == HumanKing);
        if ((Yi == 2) && (abs(Xi) == 2))
            return ((Over == ComputerPawn) || (Over == ComputerKing)) && (Piece == HumanKing);
        return false;
    }
    if ((Piece == ComputerPawn) || (Piece == ComputerKing))
    {
        if (Player != Computer)
            return false;
        const int Over = ((abs(Xi) == 2) && (abs(Yi) == 2)) ? BordArray[move.NewX - Xi/2][move.NewY - Yi/2] : Empty;
        // forwards (down) a step, or a jump over a human piece
        if ((Yi == 1) && (abs(Xi) == 1))
            return true;
        if ((Yi == 2) && (abs(Xi) == 2))
            return (Over == HumanPawn) || (Over == HumanKing);
        // backwards only a king
        if ((Yi == -1) && (abs(Xi) == 1))
            return (Piece == ComputerKing);
        if ((Yi == -2) && (abs(Xi) == 2))
            return ((Over == HumanPawn) || (Over == HumanKing)) && (Piece == ComputerKing);
        return false;
    }
    return false;
}

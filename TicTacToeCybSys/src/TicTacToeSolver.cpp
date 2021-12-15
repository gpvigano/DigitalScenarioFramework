//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "TicTacToeCybSys/TicTacToeData.h"
#include "TicTacToeCybSys/TicTacToeSolver.h"

#include <set>
#include <algorithm>

using namespace tictactoe_cybsys;

namespace
{
	void CheckBoardEmptyOrFull(GameBoard& board)
	{
		int count = 0;
		for (const auto& cell : board.Cells)
		{
			if (cell != EMPTY) count++;
		}
		board.Empty = (count == 0);
		board.Full = (count == 9);
	}

	PlayerId SolveAndFindWinner(GameInfo& game)
	{
		GameBoard& board = game.Board;
		if (board.Empty) return PlayerId::NONE;

		game.CanWin[0] = game.CanWin[1] = 0;
		game.WinningMoves.clear();
		game.SavingMoves.clear();
		int currActivePlayer = (int)game.ActivePlayer;
		int nextActivePlayer = (currActivePlayer + 1) % 2;
		static int winConfiguration[8][3] = {
			{ 0,1,2 },
			{ 3,4,5 },
			{ 6,7,8 },
			{ 0,3,6 },
			{ 1,4,7 },
			{ 2,5,8 },
			{ 0,4,8 },
			{ 2,4,6 },
		};

		// go through all winning configurations and find winners or possible winners
		for (int i = 0; i < 8; i++)
		{
			int count[2] = { 0, 0 };
			int countAlsoEmpty[2] = { 0, 0 };
			int lastEmpty = -1;
			for (int j = 0; j < 3; j++)
			{
				int pos = winConfiguration[i][j];
				CellState cell = board.Cells[pos];
				if (cell == EMPTY) lastEmpty = pos;
				if (cell != EMPTY) count[(int)cell]++;
				if (cell != PLAYER_X) countAlsoEmpty[(int)PLAYER_C]++;
				if (cell != PLAYER_C) countAlsoEmpty[(int)PLAYER_X]++;
			}

			// the winner is a player who filled 3 contiguous cells

			if (count[0] == 3)
			{
				game.CanWin[0] = game.CanWin[1] = 0;
				return PlayerId::PLAYER1;
			}
			if (count[1] == 3)
			{
				game.CanWin[0] = game.CanWin[1] = 0;
				return PlayerId::PLAYER2;
			}

			// detect moves that lead to a victory or a defeat for each player

			if (game.Started() && lastEmpty >= 0)
			{
				if (count[currActivePlayer] == 2
					&& countAlsoEmpty[currActivePlayer] == 3
					&& std::find(game.WinningMoves.begin(),game.WinningMoves.end(),lastEmpty)==game.WinningMoves.end())
				{
					game.WinningMoves.push_back(lastEmpty);
					game.CanWin[currActivePlayer]++;
				}
				if (count[nextActivePlayer] == 2
					&& countAlsoEmpty[nextActivePlayer] == 3
					&& std::find(game.SavingMoves.begin(),game.SavingMoves.end(),lastEmpty)==game.SavingMoves.end())
				{
					game.SavingMoves.push_back(lastEmpty);
					game.CanWin[nextActivePlayer]++;
				}
			}
		}
		return PlayerId::NONE;
	}
}


void tictactoe_cybsys::SolveGame(GameInfo& game)
{
	game.WinningMoves.clear();
	game.SavingMoves.clear();
	game.Winner = SolveAndFindWinner(game);
	if (game.Winner != PlayerId::NONE)
	{
		game.Ended = true;
	}
	CheckBoardEmptyOrFull(game.Board);
	if (game.Board.Full && game.Winner == PlayerId::NONE)
	{
		game.ActivePlayer = PlayerId::NONE;
		game.Ended = true;
	}
	if (!game.Ended)
	{
		game.ActivePlayer = game.ActivePlayer == PlayerId::PLAYER1 ? PlayerId::PLAYER2 : PlayerId::PLAYER1;
	}
}

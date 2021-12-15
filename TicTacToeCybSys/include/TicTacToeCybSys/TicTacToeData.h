//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <memory>
#include <vector>
#include <map>

namespace tictactoe_cybsys
{
	/*!
	Game board cell state.
	*/
	enum CellState
	{
		PLAYER_X = 0, //!< X mark.
		PLAYER_C = 1, //!< O mark.
		EMPTY, //!< No mark.
	};

	/*!
	Player identifier
	*/
	enum class PlayerId
	{
		NONE = -1,
		PLAYER1 = 0,
		PLAYER2 = 1
	};


	/*!
	Game board data.
	*/
	struct GameBoard
	{
		CellState Cells[9] = { EMPTY };
		bool Empty = true;
		bool Full = false;

		GameBoard() {}
		void Reset() { for (auto& cell : Cells) { cell = EMPTY; } }

		char CellToMark(CellState cellState)
		{
			switch (cellState)
			{
			case tictactoe_cybsys::PLAYER_X:
				return 'X';
			case tictactoe_cybsys::PLAYER_C:
				return 'O';
			default:
				return ' ';
			}
		}
		std::string ToString()
		{
			std::string boardString = "   |   |   ";
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					boardString[(i*4)+j] = CellToMark(Cells[i * 3 + j]);
				}
			}
			return boardString;
		}
	};


	/*!
	Game data, computed by SolveGame().
	*/
	struct GameInfo
	{
		/*!
		Game board data.
		*/
		GameBoard Board;
		PlayerId ActivePlayer = PlayerId::NONE;
		PlayerId Winner = PlayerId::NONE;

		/*!
		Number of possible victories with the next move for each player.
		*/
		int CanWin[2] = { 0 };

		/*!
		Winning moves for the current player.
		*/
		std::vector<int> WinningMoves;

		/*!
		Moves to prevent the next player victory.
		*/
		std::vector<int> SavingMoves;

		/*!
		Game ended.
		*/
		bool Ended = false;

		GameInfo() {}
		bool Started() { return ActivePlayer != PlayerId::NONE; }
		bool IsADraw() { return Ended && Winner == PlayerId::NONE; }

		void Reset()
		{
			Board.Reset();
			WinningMoves.clear();
			SavingMoves.clear();
			ActivePlayer = PlayerId::NONE;
			Winner = PlayerId::NONE;
			CanWin[0] = CanWin[1] = 0;
			Ended = false;
		}
	};

	/*!
	Convert player enumeration to its string identifier.
	*/
	inline std::string PlayerToString(PlayerId player)
	{
		std::string winnerStr = "none";
		if (player == PlayerId::PLAYER1) winnerStr = "player1";
		if (player == PlayerId::PLAYER2) winnerStr = "player2";
		return winnerStr;
	};

	/*!
	Convert a string identifier to player enumeration.
	*/
	inline PlayerId StringToPlayer(const std::string& playerStr)
	{
		PlayerId player = PlayerId::NONE;
		if (playerStr == "player1") player = PlayerId::PLAYER1;
		if (playerStr == "player2") player = PlayerId::PLAYER2;
		return player;
	};
}


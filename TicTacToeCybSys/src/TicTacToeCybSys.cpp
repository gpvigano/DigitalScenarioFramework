//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <TicTacToeCybSys/TicTacToeCybSys.h>
#include <TicTacToeCybSys/TicTacToeSolver.h>

#include <discenfw/xp/EnvironmentModel.h>

#include <iostream>
#include <sstream>
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT




namespace
{
	inline std::string BoolToString(bool val) { return val ? "true" : "false"; }
	inline bool StringToBool(const std::string& str) { return str == "true"; }
}

namespace tictactoe_cybsys
{
	using namespace discenfw::xp;

	TicTacToeCybSys::TicTacToeCybSys()
	{
		Game = std::make_unique<tictactoe_cybsys::GameInfo>();
	}


	void TicTacToeCybSys::CreateEntityStateTypes()
	{
		BoardEntityType = CreateEntityStateType(
			"",
			"Board",
			{
				{"state","   |   |   "},
			},
			{
			},
			{}
		);
	}


	void TicTacToeCybSys::ClearSystem()
	{
		Game->Reset();
	}


	void TicTacToeCybSys::InitFailureConditions()
	{
	}


	void TicTacToeCybSys::InitRoles()
	{
		FeatureCondition player1wins({ "winner","player1" });
		FeatureCondition player2wins({ "winner","player2" });
		FeatureCondition ended({ "ended","true" });
		FeatureCondition noWinner({ "winner","none" });
		Condition player1SuccessCond({player1wins});
		Condition player2SuccessCond({player2wins});
		Condition deadlockCond({ ended, noWinner });

		const StateRewardRules stateReward1(
			// ResultReward
		{
			{ ActionResult::IN_PROGRESS,0 },
			{ ActionResult::SUCCEEDED,1000 },
			{ ActionResult::FAILED,-1000 },
			{ ActionResult::DEADLOCK,-10 },
		},
			// PropertyCountRewards
		{},
			// EntityConditionRewards
		{
		},
			// FeatureRewards
		{
			{ { "can win 1","1" }, 25 },
			{ { "can win 2","1" }, -50 },
			{ { "can win 1",">","1" }, 50 },
			{ { "can win 2",">","1" }, -100 },
		}
		);
		SetRole(
			"player1",
			player1SuccessCond, // SuccessCondition
			player2SuccessCond, // FailureCondition
			deadlockCond, // DeadlockCondition
			stateReward1
			);

		const StateRewardRules stateReward2(
			// ResultReward
		{
			{ ActionResult::IN_PROGRESS,0 },
			{ ActionResult::SUCCEEDED,1000 },
			{ ActionResult::FAILED,-1000 },
			{ ActionResult::DEADLOCK,-10 },
		},
			// PropertyCountRewards
		{},
			// EntityConditionRewards
		{
		},
			// FeatureRewards
		{
			{ { "can win 1","1" }, -50 },
			{ { "can win 2","1" }, 25 },
			{ { "can win 1",">","1" }, -100 },
			{ { "can win 2",">","1" }, 50 },
		}
		);
		SetRole(
			"player2",
			player2SuccessCond, // SuccessCondition
			player1SuccessCond, // FailureCondition
			deadlockCond, // DeadlockCondition
			stateReward2
			);
	}


	void TicTacToeCybSys::ResetSystem()
	{
		Game->Reset();
	}


	bool TicTacToeCybSys::ExecuteAction(const Action& action)
	{
		if (action.TypeId == "move" && action.Params.size() == 2)
		{
			return DoMoveAction(action);
		}
		return false;
	}


	void TicTacToeCybSys::SynchronizeState(std::shared_ptr<xp::EnvironmentState> environmentState)
	{
		std::shared_ptr<EntityState> gameState = environmentState->GetEntityState("board");
		if (!gameState)
		{
			gameState = EntityState::Make(BoardEntityType->GetTypeName(), BoardEntityType->GetModelName());
			environmentState->SetEntityState("board", gameState);
		}

		gameState->SetPropertyValue("state", Game->Board.ToString());

		environmentState->SetFeature("started", BoolToString(Game->Started()));
		environmentState->SetFeature("ended", BoolToString(Game->Ended));
		environmentState->SetFeature("winner", PlayerToString(Game->Winner));
		environmentState->SetFeature("player", PlayerToString(Game->ActivePlayer));
		environmentState->SetFeature("can win 1", std::to_string(Game->CanWin[0]));
		environmentState->SetFeature("can win 2", std::to_string(Game->CanWin[1]));
	}


	const std::vector<ActionRef>& TicTacToeCybSys::GetAvailableActions(
		const std::string& roleId,
		bool smartSelection
		) const
	{
		CachedAvailableActions.clear();
		if (Game->Ended)
		{
			// return an empty list
			return CachedAvailableActions;
		}

		PlayerId player = StringToPlayer(roleId);
		if (Game->Started() && Game->ActivePlayer != player || player == PlayerId::NONE)
		{
			// return an empty list
			return CachedAvailableActions;
		}
		std::string playerParam = std::to_string((int)player + 1);

		// With smart selection use some heuristics to narrow down action choices
		if (smartSelection)
		{
			// if there are winning move force to use them
			if (!Game->WinningMoves.empty())
			{
				for (size_t i = 0; i < Game->WinningMoves.size(); i++)
				{
					std::string posStr = std::to_string(Game->WinningMoves[i] + 1);
					CacheAvailableAction({ "move", { posStr, playerParam } });
				}
				return CachedAvailableActions;
			}

			// if there are moves that prevent the opponent to win force to use them
			if (!Game->SavingMoves.empty())
			{
				for (size_t i = 0; i < Game->SavingMoves.size(); i++)
				{
					std::string posStr = std::to_string(Game->SavingMoves[i] + 1);
					CacheAvailableAction({ "move", { posStr, playerParam } });
				}
				return CachedAvailableActions;
			}
		}

		// select moves to free cells on the game board
		for (int i = 0; i < 9; i++)
		{
			if (Game->Board.Cells[i] == EMPTY)
			{
				std::string posStr = std::to_string(i + 1);
				CacheAvailableAction({ "move", { posStr, playerParam } });
			}
		}
		return CachedAvailableActions;
	}


	bool TicTacToeCybSys::DoMoveAction(const Action& action)
	{
		int pos = action.Params[0][0] - '1';
		int player = action.Params[1][0] - '1';
		if (pos < 0 || pos >= 9 || Game->Board.Cells[pos] != EMPTY)
		{
			return false;
		}
		if (player != (int)Game->ActivePlayer && Game->Started())
		{
			return false;
		}
		Game->ActivePlayer = (PlayerId)player;
		Game->Board.Cells[pos] = (CellState)player;
		SolveGame(*Game);
		//if (Game->Ended && Game->ActivePlayer == PlayerId::NONE)
		//{
		//	action.Result = ActionResult::DEADLOCK;
		//}
		return true;
	}


	bool TicTacToeCybSys::SetConfiguration(const std::string& config)
	{
		return true;
	}


	const std::string TicTacToeCybSys::GetConfiguration()
	{
		return "";
	}


	const std::string TicTacToeCybSys::ReadEntityConfiguration(const std::string& entityId)
	{
		return "";
	}


	bool TicTacToeCybSys::WriteEntityConfiguration(const std::string& entityId, const std::string& config)
	{
		return true;
	}


	bool TicTacToeCybSys::ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config)
	{
		return false;
	}

	bool TicTacToeCybSys::RemoveEntity(const std::string& entityId)
	{
		return false;
	}


	const std::string TicTacToeCybSys::GetSystemInfo(const std::string& infoId) const
	{
		std::ostringstream oStr;
		if (infoId.empty() || infoId == "Board")
		{
			std::string mark[3] = { "|X","|O","| " };
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					oStr << mark[(int)Game->Board.Cells[i * 3 + j]];
				}
				oStr << "|\n";
			}
			//oStr << "P1=" << Game->CanWin[0] << "  P2=" << Game->CanWin[1];
			oStr << std::endl;
		}
		else if (infoId == "PossibleMoves")
		{
			std::string mark[3] = { "|x","|o","| " };
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					oStr << mark[(int)Game->Board.Cells[i * 3 + j]];
				}
				oStr << "|    ";
				for (int j = 0; j < 3; j++)
				{
					int pos = i * 3 + j;
					CellState c = Game->Board.Cells[pos];
					if (c == CellState::EMPTY) oStr << "|" << pos + 1;
					else oStr << mark[(int)c];
				}
				oStr << "|\n";
			}
			oStr << std::endl;
		}
		std::string info = oStr.str();
		return info;
	}


	// Exporting `tictactoe_cybsys::CyberSystem` variable with alias name `CyberSystem`
	// (Has the same effect as `BOOST_DLL_ALIAS(tictactoe_cybsys::CyberSystem, CyberSystem)`)
	extern "C" BOOST_SYMBOL_EXPORT TicTacToeCybSys CyberSystem;
	TicTacToeCybSys CyberSystem;


} // namespace tictactoe_cybsys


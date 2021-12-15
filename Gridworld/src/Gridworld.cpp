//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <Gridworld/Gridworld.h>
#include <Gridworld/GridData.h>

#include <discenfw/xp/EnvironmentModel.h>

#include <iostream>
#include <sstream>
#include <algorithm>

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT



namespace gridworld_cybsys
{


	using namespace discenfw;
	using namespace discenfw::xp;


	Gridworld::Gridworld()
	{
		Grid = std::make_unique<GridData>();
	}


	void Gridworld::InitFailureConditions()
	{
	    // The failure condition is included in the default role "pawn",
	    // see InitRoles() method
	}


	void Gridworld::ResetSystem()
	{
		Position = StartPosition;
		Trajectory.clear();
		Trajectory.push_back(StartPosition);
		Bonus = 0;
	}


	bool Gridworld::ExecuteAction(const Action& action)
	{
		int maxRowPos = Grid->GetNumRows() - 1;
		int maxColPos = Grid->GetNumColumns() - 1;

		auto saveAction = [&]()
		{
			Trajectory.push_back(Position);
			if (Grid->GetCell(Position) == BONUS)
			{
				Bonus++;
			}
		};

		if (action.TypeId == "right" && (int)Position.Column < maxColPos
			&& Grid->GetCell(Position.Column + 1, Position.Row) != WALL)
		{
			Position.Column++;
			saveAction();
			return true;
		}

		if (action.TypeId == "left" && Position.Column > 0
			&& Grid->GetCell(Position.Column - 1, Position.Row) != WALL)
		{
			Position.Column--;
			saveAction();
			return true;
		}

		if (action.TypeId == "down" && (int)Position.Row < maxRowPos
			&& Grid->GetCell(Position.Column, Position.Row + 1) != WALL)
		{
			Position.Row++;
			saveAction();
			return true;
		}

		if (action.TypeId == "up" && Position.Row > 0
			&& Grid->GetCell(Position.Column, Position.Row - 1) != WALL)
		{
			Position.Row--;
			saveAction();
			return true;
		}

		return false;
	}


	void Gridworld::SynchronizeState(std::shared_ptr<xp::EnvironmentState> environmentState)
	{
		if (!Grid->IsValid())
		{
			return;
		}

		environmentState->SetFeature("Bonus", std::to_string(Bonus));

		std::shared_ptr<EntityState> entState = environmentState->GetEntityState("pawn");
		if (!entState)
		{
			entState = EntityState::Make(PawnEntityType->GetTypeName(), PawnEntityType->GetModelName());
			environmentState->SetEntityState("pawn", entState);
		}

		int unexploredCellCount = CountUnexplored();

		// Get properties from the cyber system
		entState->SetPropertyValue("row", std::to_string(Position.Row));
		entState->SetPropertyValue("column", std::to_string(Position.Column));
		entState->SetPropertyValue("unexplored", std::to_string(unexploredCellCount));
		char stateChar = Grid->GetCell(Position);
		std::string stateValue;
		switch (stateChar)
		{
		case START:
			stateValue = "free";
			break;
		case END:
			stateValue = "end";
			break;
		case TRAP:
			stateValue = "trap";
			break;
		default:
			stateValue = "free";
			break;
		}
		entState->SetPropertyValue("state", stateValue);
	}


	const std::vector<ActionRef>& Gridworld::GetAvailableActions(
		const std::string& roleId,
		bool smartSelection
		) const
	{
		// empty the cache
		CachedAvailableActions.clear();

		// fill available actions for the current system state

		int maxRowPos = Grid->GetNumRows() - 1;
		int maxColPos = Grid->GetNumColumns() - 1;

		bool canGoRight = (int)Position.Column < maxColPos
			&& Grid->GetCell(Position.Column + 1, Position.Row) != WALL;

		bool canGoLeft = (int)Position.Column > 0
			&& Grid->GetCell(Position.Column - 1, Position.Row) != WALL;

		bool canGoDown = (int)Position.Row < maxRowPos
			&& Grid->GetCell(Position.Column, Position.Row + 1) != WALL;

		bool canGoUp = (int)Position.Row > 0
			&& Grid->GetCell(Position.Column, Position.Row - 1) != WALL;

		// If smartSelection apply a simple heuristic: do not move back.
		if (smartSelection)
		{
			canGoRight = canGoRight && !IsVisited(Position.Column + 1, Position.Row);
			canGoLeft = canGoLeft && !IsVisited(Position.Column - 1, Position.Row);
			canGoDown = canGoDown && !IsVisited(Position.Column, Position.Row + 1);
			canGoUp = canGoUp && !IsVisited(Position.Column, Position.Row - 1);
		}

		if (canGoRight)
		{
			CacheAvailableAction({ "right" });
		}
		if (canGoLeft)
		{
			CacheAvailableAction({ "left" });
		}
		if (canGoDown)
		{
			CacheAvailableAction({ "down" });
		}
		if (canGoUp)
		{
			CacheAvailableAction({ "up" });
		}

		return CachedAvailableActions;
	}


	void Gridworld::InitRoles()
	{
		PropertyCondition allExplored({ "unexplored","0" });
		PropertyCondition ended({ "state","end" });
		PropertyCondition trapped({ "state","trap" });
		Condition successCond({ { "pawn",{ ended } } });
		Condition failureCond({ { "pawn",{ trapped } } });
		Condition deadlockCond({ { "pawn",{ allExplored } } });

		const StateRewardRules stateReward(
			// ResultReward
			{
				{ ActionResult::IN_PROGRESS,-1 },
				{ ActionResult::SUCCEEDED,100 },
				{ ActionResult::FAILED,-100 },
				{ ActionResult::DEADLOCK,-10 },
			},
			// PropertyCountRewards
			{},
			// EntityConditionRewards
			{},
			// FeatureRewards
			{
				{ { "Bonus","","" }, 25 },
			}
		);

		SetRole(
			"Pawn",
			successCond, // SuccessCondition
			failureCond, // FailureCondition
			deadlockCond, // DeadlockCondition
			stateReward
			);
	}


	bool Gridworld::SetConfiguration(const std::string& config)
	{
		if (config.empty())
		{
			return false;
		}
		unsigned row = 0;
		unsigned rows = 0;
		unsigned columns = 0;
		bool dimFound = false;
		std::string gridConfig = config;
		if (gridConfig.back() != '\n')
		{
			gridConfig.push_back('\n');
		}
		std::istringstream iStr(gridConfig);
		while (iStr.good())
		{
			std::string line;
			std::getline(iStr, line);
			if (line.empty())
			{
				break;
			}
			if (!dimFound)
			{
				std::istringstream lineInStr(line);
				lineInStr >> columns >> rows;
				Grid->SetDimension(columns, rows);
				dimFound = true;
			}
			else
			{
				Grid->SetRow(row, line);
				size_t startPos = line.find(START);
				if (startPos != std::string::npos)
				{
					StartPosition.Row = row;
					StartPosition.Column = (unsigned)startPos;
				}
				row++;
			}
		}

		return true;
	}


	const std::string Gridworld::GetConfiguration()
	{
		if (!Grid->IsValid())
		{
			return "";
		}
		std::ostringstream oStr;
		oStr << Grid->GetNumColumns() << Grid->GetNumRows() << "\n";
		for (unsigned row = 0U; row < Grid->GetNumRows(); row++)
		{
			oStr << Grid->GetRow(row) << "\n";
		}
		return oStr.str();
	}


	const std::string Gridworld::ReadEntityConfiguration(const std::string& entityId)
	{
		return "";
	}


	bool Gridworld::WriteEntityConfiguration(const std::string& entityId, const std::string& config)
	{
		return true;
	}


	bool Gridworld::ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config)
	{
		return true;
	}

	bool Gridworld::RemoveEntity(const std::string& entityId)
	{
		return false;
	}


	const std::string Gridworld::GetSystemInfo(const std::string& infoId) const
	{
		std::ostringstream oStr;
		if (infoId == "") // default info
		{
			oStr << " ";
			for (unsigned col = 0U; col < Grid->GetNumColumns(); col++)
			{
				oStr << "_";
			}
			oStr << " \n";
			for (unsigned row = 0U; row < Grid->GetNumRows(); row++)
			{
				oStr << "|";
				std::string line = Grid->GetRow(row);
				for (size_t col = 0; col < line.size(); col++)
				{
					bool atCurrPos = (row == Position.Row && col == Position.Column);
					if (atCurrPos)
					{
						oStr << "*";
					}
					else
					{
						if (IsVisited((unsigned)col, (unsigned)row))
						{
							switch (line[col])
							{
							case START:
								oStr << ":";
								break;
							case BONUS:
								oStr << ";";
								break;
							default:
								oStr << ".";
								break;
							}
						}
						else
						{
							oStr << line[col];
						}
					}
				}
				oStr << "|\n";
			}
			oStr << "'";
			for (unsigned col = 0U; col < Grid->GetNumColumns(); col++)
			{
				oStr << "-";
			}
			oStr << "'\n(";
			oStr << Position.Column << "," << Position.Row << ") = '" << Grid->GetCell(Position.Column, Position.Row) << "' ";
			int dircount = CountUnexplored();
			oStr << dircount << (dircount==1 ? " direction" : " directions");
			oStr << " bonus=" << Bonus;
			oStr << std::endl;
		}

		std::string info = oStr.str();
		return info;
	}



	void Gridworld::CreateEntityStateTypes()
	{
		PawnEntityType = CreateEntityStateType(
			"",
			"Position",
			{
				{ "column","0" },
				{ "row","0" },
				{ "state","free" },
				{ "unexplored","" },
			},
			{
				{ "state",{ "free","trap","end" } },
			},
			{}
		);

	}


	void Gridworld::ClearSystem()
	{
		Grid->Clear();
		Position = StartPosition = { 0, 0 };
		ResetSystem();
	}


	bool Gridworld::IsVisited(unsigned col, unsigned row) const
	{
		return IsVisited({ col, row });
	}


	bool Gridworld::IsVisited(const GridPosition& pos) const
	{
		auto itr = std::find(Trajectory.begin(), Trajectory.end(), pos);
		return itr != Trajectory.end();// && Grid->GetCell(pos) != START;
	}


	bool Gridworld::IsUnexplored(unsigned col, unsigned row) const
	{
		if (col == Position.Column && row == Position.Row)
		{
			return false;
		}
		char stateChar = Grid->GetCell(col, row);
		if (stateChar == WALL || stateChar == START)
		{
			return false;
		}
		return !IsVisited(col, row);
	}


	int Gridworld::CountUnexplored() const
	{
		int unexploredCellCount = 0;
		if (Position.Row > 0 && IsUnexplored(Position.Column, Position.Row - 1))
		{
			unexploredCellCount++;
		}
		if (Position.Row < Grid->GetNumRows() - 1 && IsUnexplored(Position.Column, Position.Row + 1))
		{
			unexploredCellCount++;
		}
		if (Position.Column > 0 && IsUnexplored(Position.Column - 1, Position.Row))
		{
			unexploredCellCount++;
		}
		if (Position.Column < Grid->GetNumColumns() - 1 && IsUnexplored(Position.Column + 1, Position.Row))
		{
			unexploredCellCount++;
		}
		return unexploredCellCount;
	}


	extern "C" BOOST_SYMBOL_EXPORT Gridworld CyberSystem;
	Gridworld CyberSystem;

}

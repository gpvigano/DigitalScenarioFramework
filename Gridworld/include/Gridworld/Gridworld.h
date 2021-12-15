//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <DiScenFw/interop/CyberSystemPlugin.h>

#include "Gridworld/GridPosition.h"

#include <memory>
#include <map>

namespace gridworld_cybsys
{
	class GridData;

	/*!
	Gridworld CyberSystemPlugin implementation.
	*/
	class Gridworld : public discenfw::xp::CyberSystemPlugin
	{
	public:

		/*!
		Characters used to mark grid cells.
		*/
		enum CellTypeEnum {
			EMPTY=' ', //! Empty/free cell.
			START='S', //! Start position.
			TRAP='!',  //! Trap.
			WALL='#',  //! Wall/obstacle.
			END='E',   //! End position.
			BONUS='$',   //! Bonus reward.
			PAWN='*',  //! Current pawn position (not used in configuration).
		};

		Gridworld();

		void SynchronizeState(std::shared_ptr<discenfw::xp::EnvironmentState> environmentState) override;

		const std::vector<discenfw::xp::ActionRef>& GetAvailableActions(
			const std::string& roleId = "",
			bool smartSelection = false
			) const override;

		void InitFailureConditions() override;

		void InitRoles() override;

		bool ExecuteAction(const discenfw::xp::Action& action) override;

		void ResetSystem() override;

		const std::string GetSystemName() const override
		{
			return "Gridworld";
		}

		const std::string GetSystemInfo(const std::string& infoId = "") const override;

		/*!
		Set the grid configuration (see CellTypeEnum).

		The required string is a multi-line text with the dimensions (columns, rows)
		in the first line and the grid layout in following rows.
		Sample string:
		@code
		std::string config =
							"4 5\n"\
							"   E\n"\
							"    \n"\
							"  # \n"\
							"    \n"\
							" S  \n";
		@endcode
		*/
		bool SetConfiguration(const std::string& config) override;

		/*!
		Get the grid configuration (see SetConfiguration()).
		*/
		const std::string GetConfiguration() override;

		const std::string ReadEntityConfiguration(const std::string& entityId) override;

		bool WriteEntityConfiguration(const std::string& entityId, const std::string& config) override;

		bool ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config) override;

		bool RemoveEntity(const std::string& entityId) override;

	protected:

		std::shared_ptr<discenfw::xp::EntityStateType> PawnEntityType;
		std::unique_ptr<GridData> Grid;

		GridPosition Position;
		GridPosition StartPosition;
		std::vector<GridPosition> Trajectory;
		int Bonus = 0;


		void CreateEntityStateTypes() override;

		virtual void ClearSystem() override;

		bool IsVisited(unsigned row, unsigned col) const;
		bool IsVisited(const GridPosition& pos) const;
		bool IsUnexplored(unsigned row, unsigned col) const;
		int CountUnexplored() const;
	};

}

//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <DiScenFwConfig.h>
#include "discenfw/xp/EnvironmentState.h"
#include "discenfw/xp/Experience.h"
#include "discenfw/xp/Action.h"
#include "discenfw/xp/Condition.h"
#include <memory>

/// @cond DOXYGEN_IGNORE

// Forward declarations

namespace discenfw
{
	namespace xp
	{
		class EnvironmentModel;
	}
}

/// @endcond


namespace discenfw
{
	namespace xp
	{
		/*!
		Interface with a cyber system.
		This enables to interact with the cyber system
		and updates the scenario state.
		*/
		class DISCENFW_API ICyberSystem
		{
		public:

			virtual ~ICyberSystem() {}


			/*!
			Synchronize the given state with the current state of the system.
			*/
			virtual void SynchronizeState(std::shared_ptr<EnvironmentState> environmentState) = 0;

			/*!
			Get a list of available actions for the current state of the system.
			@param roleId Optional role used to select actions
			@param smartSelection select only actions (if known) that make sense (using heuristics to prune the decision tree),
					e.g. in particular states select only actions that lead to an immediate success or prevent an immediate failure
					(if the current cyber system does not support this feature, all available actions are returned)
			@return Return List of selected actions.
			*/
			virtual const std::vector<ActionRef>& GetAvailableActions(
				const std::string& roleId = "",
				bool smartSelection = false
				) const = 0;

			/*!
			Define the common failure conditions (see discenfw::xp::Assistant::SetFailureCondition()).
			*/
			virtual void InitFailureConditions() = 0;

			/*!
			Initialize the roles.
			*/
			virtual void InitRoles() = 0;

			/*!
			Parse the action type and parameters and implement its behavior.
			@return true if recognized, false otherwise.
			*/
			virtual bool ExecuteAction(const Action& action) = 0;

			/*!
			Compute the last scenario state from the system.
			*/
			virtual const EnvironmentState& InterpretSystemState() = 0;

			/*!
			Reset the digital scenario to its original state.
			*/
			virtual void ResetSystem() = 0;

			/*!
			Get the system name.
			*/
			virtual const std::string GetSystemName() const = 0;

			/*!
			Get information from the system.
			*/
			virtual const std::string GetSystemInfo(const std::string& infoId = "") const = 0;

			/*!
			Set the digital scenario configuration.
			*/
			virtual bool SetConfiguration(const std::string& config) = 0;

			/*!
			Get the digital scenario configuration.
			*/
			virtual const std::string GetConfiguration() = 0;

			/*!
			Read the configuration of an entity from digital scenario.
			*/
			virtual const std::string ReadEntityConfiguration(const std::string& entityId) = 0;

			/*!
			Write the configuration of an entity to digital scenario.
			*/
			virtual bool WriteEntityConfiguration(const std::string& entityId, const std::string& config) = 0;

			/*!
			Write the configuration of an entity to digital scenario, create it if needed.
			*/
			virtual bool ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config) = 0;

			/*!
			Remove an entity from the system.
			*/
			virtual bool RemoveEntity(const std::string& entityId) = 0;

			/*!
			Initialize the system and use current configuration to build the initial state.
			@param rebuild Rebuild the initial state, even if already initialized.
			*/
			virtual void Initialize(bool rebuild = false) = 0;

			/*!
			Completely clear the system, set it to "not initialized".
			*/
			virtual void Clear() = 0;

			/*!
			Check if the cyber system was initialized.
			*/
			virtual bool IsInitialized() const = 0;


			/*!
			Get the intial scenario state, valid only after the first call to Initialize().
			*/
			virtual const EnvironmentState& GetInitialSystemState() = 0;

			/*!
			Get the current scenario state, valid only after the first call to InterpretSystemState().
			*/
			virtual const EnvironmentState& GetLastSystemState() = 0;

			/*!
			Get the list of system failure conditions.
			*/
			virtual const Condition& GetFailureCondition() const = 0;

			virtual void SetLogEnabled(bool enabled) = 0;

			virtual bool IsLogEnabled() const = 0;

		};

	}
}


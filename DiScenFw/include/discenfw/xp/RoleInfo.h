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

#include "discenfw/xp/Condition.h"
#include "discenfw/xp/StateRewardRules.h"
#include "discenfw/xp/EnvironmentState.h"
#include "discenfw/xp/EnvironmentStateInfo.h"
#include "discenfw/xp/Transition.h"

#include <string>
#include <map>
#include <memory>


namespace discenfw
{
	namespace xp
	{
		/*!
		Role information for an agent interacting with an environment.
		*/
		class DISCENFW_API RoleInfo
		{
		// Prevent public access from DLL clients
		DISCENFW_DLL_PROTECTED
			std::string RoleName;  //!< Name of the role.
			std::string ModelName; //!< Name of the related model.

			Condition SuccessCondition; //!< Condition for success.
			Condition FailureCondition; //!< Condition of failure.
			Condition DeadlockCondition; //!< Condition of deadlock.

			StateRewardRules StateReward; //!< Reward computed according to a set of conditions.

			RoleInfo(const std::string& roleName = "", const std::string& modelName = "");

			RoleInfo(
				const std::string& roleName,
				const Condition& successCondition,
				const Condition& failureCondition,
				const Condition& deadlockCondition,
				const StateRewardRules& stateRewardRules,
				const std::string& modelName = ""
				);


		public:


			/*!
			Make a new role (allocated in the library module), return a shared pointer to it.
			*/
			static std::shared_ptr<RoleInfo> Make(const std::string& roleName, const std::string& modelName);

			///@name Getters/setters
			///@{

			const std::string& GetRoleName() const
			{
				return RoleName;
			}

			void SetRoleName(const std::string& roleName);


			const std::string& GetModelName() const
			{
				return ModelName;
			}

			void SetModelName(const std::string& modelName);


			const Condition& GetSuccessCondition()
			{
				return SuccessCondition;
			}

			void SetSuccessCondition(const Condition& successCondition);

			void AddSuccessCondition(const Condition& successCondition);


			const Condition& GetFailureCondition()
			{
				return FailureCondition;
			}

			void SetFailureCondition(const Condition& failureCondition);

			void AddFailureCondition(const Condition& failureCondition);


			const Condition& GetDeadlockCondition()
			{
				return DeadlockCondition;
			}

			void SetDeadlockCondition(const Condition& deadlockCondition);

			void AddDeadlockCondition(const Condition& deadlockCondition);


			const StateRewardRules& GetStateReward()
			{
				return StateReward;
			}

			void SetStateReward(const StateRewardRules& stateReward);

			///@}

			/*!
			Detect success or failure for the current state and compute the reward.
			@note Failure condition is evaluated before success condition,
			thus if a failure condition is true the result is a failure despite any other success condition.
			@return information about the given state for this role.
			*/
			EnvironmentStateInfo GetStateInfo(std::shared_ptr<EnvironmentState> environmentState) const;


			/*!
			Override the state information for the given state.
			*/
			EnvironmentStateInfo OverrideStateInfo(
				std::shared_ptr<EnvironmentState> environmentState,
				const EnvironmentStateInfo& stateInfo
				) const;


			/*!
			Override the state result for the given state, reward is recalculated.
			*/
			EnvironmentStateInfo OverrideStateResult(
				std::shared_ptr<EnvironmentState> environmentState,
				const ActionResult& result
				) const;


			/*!
			Override the state reward for the given state, result is unchanged.
			*/
			EnvironmentStateInfo OverrideStateReward(
				std::shared_ptr<EnvironmentState> environmentState,
				int reward
				) const;


			/*!
			Update the state information for the given state (e.g. if rules were changed or to cancel an OverrideStateInfo() call).
			*/
			void UpdateStateInfo(std::shared_ptr<EnvironmentState> environmentState) const;


			/*!
			Clear the computed state information and the values of known transitions.
			*/
			void Clear() const;

		protected:

			mutable std::map< std::shared_ptr<EnvironmentState>, EnvironmentStateInfo > StateInfo;

			/*!
			Evaluate a state, aacording to the success, deadlock and failure conditions defined.
			*/
			EnvironmentStateInfo ComputeStateInfo(std::shared_ptr<EnvironmentState> environmentState) const;

			/*!
			Evaluate a state, aacording to the success, deadlock and failure conditions defined.
			*/
			ActionResult EvaluateStateConditions(std::shared_ptr<EnvironmentState> environmentState) const;

			/*!
			Compute the reward for a state, aacording to the given result.
			*/
			void ComputeStateReward(std::shared_ptr<EnvironmentState>, EnvironmentStateInfo& stateInfo) const;
		};

	}
}


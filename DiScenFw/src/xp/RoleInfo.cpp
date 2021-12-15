//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "discenfw/xp/RoleInfo.h"

#include <discenfw/util/CompOp.h>
#include <discenfw/util/MessageLog.h>

#include <gpvulc/text/string_conv.h>

/*
Note: pages referenced here are from the book
Sutton R.S.,Barto A.G., 2020, "Reinforcement Learning: An Introduction". Second edition
*/


namespace discenfw
{
	namespace xp
	{
		RoleInfo::RoleInfo(const std::string& roleName, const std::string& modelName) :
			RoleName(roleName),
			StateInfo(),
			ModelName(modelName)
		{
		}

		RoleInfo::RoleInfo(
			const std::string& roleName,
			const Condition& successCondition,
			const Condition& failureCondition,
			const Condition& deadlockCondition,
			const StateRewardRules& stateRewardRules,
			const std::string& modelName
			) :
			StateInfo(),
			RoleName(roleName),
			SuccessCondition(successCondition),
			FailureCondition(failureCondition),
			DeadlockCondition(deadlockCondition),
			StateReward(stateRewardRules),
			ModelName(modelName)
		{
		}


		std::shared_ptr<RoleInfo> RoleInfo::Make(const std::string& roleName, const std::string& modelName)
		{
			return std::make_shared<RoleInfo>(roleName, modelName);
		}


		void RoleInfo::SetRoleName(const std::string& roleName)
		{
			RoleName = roleName;
			Clear();
		}


		void RoleInfo::SetModelName(const std::string& modelName)
		{
			ModelName = modelName;
			Clear();
		}


		void RoleInfo::SetSuccessCondition(const Condition& successCondition)
		{
			SuccessCondition = successCondition;
			Clear();
		}

		void RoleInfo::AddSuccessCondition(const Condition & successCondition)
		{
			if (!SuccessCondition.Defined())
			{
				SuccessCondition = successCondition;
			}
			else
			{
				SuccessCondition.AddCondition(LogicOp::AND, successCondition);
			}
		}


		void RoleInfo::SetFailureCondition(const Condition& failureCondition)
		{
			FailureCondition = failureCondition;
			Clear();
		}

		void RoleInfo::AddFailureCondition(const Condition& failureCondition)
		{
			if (!FailureCondition.Defined())
			{
				FailureCondition = failureCondition;
			}
			else
			{
				FailureCondition.AddCondition(LogicOp::OR, failureCondition);
			}
		}


		void RoleInfo::SetDeadlockCondition(const Condition& deadlockCondition)
		{
			DeadlockCondition = deadlockCondition;
			Clear();
		}

		void RoleInfo::AddDeadlockCondition(const Condition & deadlockCondition)
		{
			if (!DeadlockCondition.Defined())
			{
				DeadlockCondition = deadlockCondition;
			}
			else
			{
				DeadlockCondition.AddCondition(LogicOp::OR, deadlockCondition);
			}
		}


		void RoleInfo::SetStateReward(const StateRewardRules& stateReward)
		{
			StateReward = stateReward;
			Clear();
		}


		EnvironmentStateInfo RoleInfo::GetStateInfo(std::shared_ptr<EnvironmentState> environmentState) const
		{
			if (StateInfo.find(environmentState) == StateInfo.end()
				|| !StateReward.FeatureRewards.empty())
			{
				return ComputeStateInfo(environmentState);
			}
			return StateInfo[environmentState];
		}


		EnvironmentStateInfo RoleInfo::OverrideStateInfo(
			std::shared_ptr<EnvironmentState> environmentState,
			const EnvironmentStateInfo& stateInfo
			) const
		{
			if (environmentState)
			{
				StateInfo[environmentState] = stateInfo;
			}
			else
			{
				LogMessage(LOG_ERROR, "Role " + RoleName + " - Cannot update with a null state.", "DiScenFw");
			}
			return stateInfo;
		}


		EnvironmentStateInfo RoleInfo::OverrideStateResult(
			std::shared_ptr<EnvironmentState> environmentState,
			const ActionResult& result
			) const
		{
			if (environmentState)
			{
				GetStateInfo(environmentState);
				StateInfo[environmentState].Result = result;
				ComputeStateReward(environmentState, StateInfo[environmentState]);
				// TODO: Compute state interest (p.234): terminal=1, deadlock=0, trajectory-to-terminal>0
			}
			else
			{
				LogMessage(LOG_ERROR, "Role " + RoleName + " - Cannot override with a null state.", "DiScenFw");
			}

			return StateInfo[environmentState];
		}


		EnvironmentStateInfo RoleInfo::OverrideStateReward(std::shared_ptr<EnvironmentState> environmentState, int reward) const
		{
			if (environmentState)
			{
				GetStateInfo(environmentState);
				StateInfo[environmentState].Reward = reward;
			}
			else
			{
				LogMessage(LOG_ERROR, "Role " + RoleName + " - Cannot update reward of a null state.", "DiScenFw");
				return EnvironmentStateInfo();
			}

			return StateInfo[environmentState];
		}


		void RoleInfo::UpdateStateInfo(std::shared_ptr<EnvironmentState> environmentState) const
		{
			if (environmentState)
			{
				ComputeStateInfo(environmentState);
			}
			else
			{
				LogMessage(LOG_ERROR, "Role " + RoleName + " - Cannot update info with a null state.", "DiScenFw");
			}
		}



		void RoleInfo::Clear() const
		{
			StateInfo.clear();
		}



		EnvironmentStateInfo RoleInfo::ComputeStateInfo(std::shared_ptr<EnvironmentState> environmentState) const
		{
			EnvironmentStateInfo stateInfo;
			stateInfo.Result = EvaluateStateConditions(environmentState);
			ComputeStateReward(environmentState, stateInfo);
			// TODO: Compute state interest (p.234)
			StateInfo[environmentState] = stateInfo;
			return stateInfo;
		}


		ActionResult RoleInfo::EvaluateStateConditions(std::shared_ptr<EnvironmentState> environmentState) const
		{
			if (FailureCondition.Evaluate(environmentState))
			{
				return ActionResult::FAILED;
			}
			else if (SuccessCondition.Evaluate(environmentState))
			{
				return ActionResult::SUCCEEDED;
			}
			else if (DeadlockCondition.Defined()
				&& DeadlockCondition.Evaluate(environmentState))
			{
				return ActionResult::DEADLOCK;
			}

			return ActionResult::IN_PROGRESS;
		}


		void RoleInfo::ComputeStateReward(std::shared_ptr<EnvironmentState> environmentState, EnvironmentStateInfo& stateInfo) const
		{
			/*
			The reward computation must be external to the agent.
			(See Sutton R.S.,Barto A.G., 2020, "Reinforcement Learning: An Introduction".
			Second edition. p.50)
			*/

			stateInfo.Reward = 0;

			// in-progress-reward is used to compute a cumulative reward at the end of the episode
			if (stateInfo.Result != ActionResult::IN_PROGRESS
				&& StateReward.ResultReward.find(stateInfo.Result)!=StateReward.ResultReward.cend())
			{
				stateInfo.Reward = StateReward.ResultReward.at(stateInfo.Result);
			}

			if (stateInfo.Result == ActionResult::FAILED)
			{

				// For failures there is no other evaluation
				return;
			}

			// Add the proper reward for each satisfied entity condition

			int entityPropertiesReward = 0;
			for (const auto& item : StateReward.EntityConditionRewards)
			{
				bool eval = item.first.Evaluate(environmentState);
				if (eval)
				{
					entityPropertiesReward += item.second;
				}
			}

			// Add the proper reward for each satisfied condition on features

			int featuresReward = 0;
			for (const auto& item : StateReward.FeatureRewards)
			{
				const FeatureCondition& cond = item.first;
				bool eval = cond.Evaluate(*environmentState);
				if (eval)
				{
					int reward = item.second;
					if (cond.ComparisonOperator == CompOp::DEFINED)
					{
						std::string feature = environmentState->GetFeature(cond.FeatureName);
						if (!feature.empty())
						{
							// try to convert the feature value to a number

							try
							{
								int featReward = std::stoi(feature);

								// if it was a number use it as multiplier for the reward
								reward *= featReward;
							}
							catch (...)
							{
								// else continue as usual
							}
						}
					}
					featuresReward += reward;
				}
			}

			// Add the proper reward for each property matched by each entity

			int cumulativeReward = 0;
			for (const auto& entityState : environmentState->EntityStates)
			{
				for (const auto& item : StateReward.CumulativeRewards)
				{
					if(entityState.second->IsOfType(item.TypeName))
					{
						bool eval = item.Filter.Evaluate(*(entityState.second));
						if (eval)
						{
							cumulativeReward += item.Reward;
						}
					}
				}
			}

			stateInfo.Reward += cumulativeReward + entityPropertiesReward + featuresReward;
		}

	}
}


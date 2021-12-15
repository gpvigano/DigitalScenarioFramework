//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/Experience.h>

#include <discenfw/xp/EnvironmentModel.h>
#include <discenfw/xp/RoleInfo.h>

#include <algorithm>

namespace discenfw
{
	namespace xp
	{
		const std::shared_ptr<RoleInfo> Experience::GetRole() const
		{
			const std::shared_ptr<RoleInfo> role = GetModel()->GetRole(Role);
			return role;
		}

		const std::string& Experience::GetRoleName() const
		{
			const std::shared_ptr<RoleInfo> role = GetModel()->GetRole(Role);
			if (Role.empty() && role)
			{
				return role->GetRoleName();
			}
			return Role;
		}


		const std::shared_ptr<EnvironmentModel> Experience::GetModel() const
		{
			return xp::GetModel(Model);
		}


		const std::string& Experience::GetModelName() const
		{
			return GetModel()->GetName();
		}


		int Experience::IndexOfState(const std::shared_ptr<EnvironmentState> state) const
		{
			int idx = GetModel()->IndexOfState(state);
			return idx;
		}


		std::shared_ptr<EnvironmentState> Experience::FindState(
			const std::shared_ptr<EnvironmentState> environmentState) const
		{
			std::shared_ptr<EnvironmentState> foundScenarioState = GetModel()->FindState(environmentState);
			return foundScenarioState;
		}


		std::shared_ptr<EnvironmentState> Experience::GetStoredState(
			const std::shared_ptr<EnvironmentState> environmentState) const
		{
			std::shared_ptr<EnvironmentState> foundScenarioState = GetModel()->GetStoredState(environmentState);
			return foundScenarioState;
		}


		std::shared_ptr<EnvironmentState> Experience::GetStoredState(const EnvironmentState& environmentState) const
		{
			std::shared_ptr<EnvironmentState> foundScenarioState = GetModel()->GetStoredState(environmentState);
			return foundScenarioState;
		}


		std::shared_ptr<EnvironmentState> Experience::GetStoredState(int stateIndex) const
		{
			std::shared_ptr<EnvironmentState> foundScenarioState = GetModel()->GetStoredState(stateIndex);
			return foundScenarioState;
		}


		void Experience::StoreState(const std::shared_ptr<EnvironmentState> environmentState)
		{
			GetModel()->GetStoredState(environmentState);
		}


		EnvironmentStateInfo Experience::GetStateInfo(const std::shared_ptr<EnvironmentState> environmentState)
		{
			EnvironmentStateInfo info = GetRole()->GetStateInfo(GetModel()->GetCurrentState());
			return info;
		}

		bool Experience::CheckDuplicateEpisode(std::shared_ptr<Episode> episode)
		{
			for (size_t i = 0; i < Episodes.size(); i++)
			{
				if (Episodes[i]->InitialState != episode->InitialState)
				{
					continue;
				}
				if (Episodes[i]->LastState != episode->LastState)
				{
					continue;
				}
				if (Episodes[i]->Result != episode->Result)
				{
					continue;
				}
				if (Episodes[i]->TransitionSequence.size() != episode->TransitionSequence.size())
				{
					continue;
				}
				bool diffFound = false;
				for (size_t t = 0; !diffFound && t < episode->TransitionSequence.size(); t++)
				{
					if (Episodes[i]->TransitionSequence[t].StartState != episode->TransitionSequence[t].StartState)
					{
						diffFound = true;
					}
					else if (Episodes[i]->TransitionSequence[t].EndState != episode->TransitionSequence[t].EndState)
					{
						diffFound = true;
					}
					else if (Episodes[i]->TransitionSequence[t].ActionTaken->TypeId != episode->TransitionSequence[t].ActionTaken->TypeId)
					{
						diffFound = true;
					}
					else if (Episodes[i]->TransitionSequence[t].ActionTaken->Params != episode->TransitionSequence[t].ActionTaken->Params)
					{
						diffFound = true;
					}
				}
				if (!diffFound)
				{
					Episodes[i]->RepetitionsCount++;
					return true;
				}
			}
			return false;
		}


		bool Experience::StoreEpisode(std::shared_ptr<Episode> episode, bool checkDuplicate)
		{
			if (checkDuplicate && CheckDuplicateEpisode(episode))
			{
				return false;
			}
			// if the episode was a failure store only the last transition
			if (episode->Failed() && !episode->TransitionSequence.empty())
			{
				const Transition& lastTransition = episode->TransitionSequence.back();
				const auto& failedTransitions = FailedTransitions;
				if (std::find(failedTransitions.begin(), failedTransitions.end(), lastTransition)
					== failedTransitions.end())
				{
					FailedTransitions.push_back(lastTransition);
				}
			}

			Episodes.push_back(episode);
			if (episode->Succeded())
			{
				if (!BestEpisode)
				{
					BestEpisode = episode;
					BestEpisodes.push_back(BestEpisode);
				}
				else if(episode->Performance >= BestEpisode->Performance)
				{
					if (episode->Performance > BestEpisode->Performance)
					{
						BestEpisodes.clear();
					}
					BestEpisode = episode;
					BestEpisodes.push_back(BestEpisode);
				}
			}

			return true;
		}


		bool Experience::StateActionValueDefined(const StateActionRef& stateAction) const
		{
			return StateActionValues.find(stateAction)!=StateActionValues.cend();
		}


		float Experience::GetStateActionValue(const StateActionRef& stateAction) const
		{
			if (StateActionValueDefined(stateAction))
			{
				return StateActionValues.at(stateAction);
			}
			// for new entries the default value is always 0
			return 0.0f;
		}


		void Experience::SetStateActionValue(const StateActionRef& stateAction, float actionValue)
		{
			if (actionValue==0.0f && !StateActionValueDefined(stateAction))
			{
				// for new entries the default value is 0 anyway (save memory)
				return;
			}
			StateActionValues[stateAction] = actionValue;
		}


		const std::map<StateActionRef, float>& Experience::GetStateActionValues() const
		{
			return StateActionValues;
		}


		void Experience::ClearStateActionValues()
		{
			StateActionValues.clear();
		}


		void Experience::Clear()
		{
			if (!Role.empty())
			{
				GetRole()->Clear();
			}
			if (!Model.empty())
			{
				GetModel()->ClearStoredStates();
			}
			BestEpisode = nullptr;
			BestEpisodes.clear();
			Episodes.clear();
			StateActionValues.clear();
			FailedTransitions.clear();
			Level = ExperienceLevel::NONE;
		}
	}
}

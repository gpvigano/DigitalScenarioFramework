//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <SampleAgent/SampleAgent.h>

#include <discenfw/xp/Experience.h>

#include <discenfw/util/MessageLog.h>
#include <discenfw/util/Rand.h>

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT

#include <iostream>
#include <math.h>

namespace sample_agent
{

	using namespace discenfw;
	using namespace discenfw::xp;


	// TODO: define a namespace if needed


	SampleAgent::SampleAgent()
	{
		Configuration = std::make_shared<SampleAgentConfig>();
	}


	SampleAgent::~SampleAgent()
	{
	}


	void SampleAgent::Reset()
	{
		// TODO: clear and reset data

		ActionChoiceCount = 0;
		ExplorationActionCount = 0;
		KnownActions.clear();
		SuccessActions.clear();
	}


	int SampleAgent::ChooseAction(
		const std::shared_ptr<Experience> experience,
		const std::vector<ActionRef>& possibleActions,
		StateRef envState,
		bool updatePolicy
		)
	{
		if (possibleActions.empty())
		{
			return -1;
		}

		if (updatePolicy)
		{
			// Update your policy/policies here
		}

		int chosenAction = -1;

		StateActionRef stateAction;
		stateAction.State = envState;

		// choose an action (index) according to a policy

		// TODO: test a rollout policy

		// Use a Boltzmann distribution for policy

		// temperature: high values make the probability
		// distribution virtually uniform, while low values
		// make the policy nearly a greedy selection.
		float temperature = Configuration->NewActionValue;
		if (temperature < 1.0f)
		{
			temperature = 1.0f;
		}

		// Accumulator of probability distribution:
		// each value represent an upper limit for the range related
		// to the probability of choosing an action.
		// A random number (0..1) will select an action.
		std::vector<float> distribution(possibleActions.size(), 0.0f);
		float denominatorBoltzmann = 0.0f;
		for (int i = 0; i < (int)possibleActions.size(); i++)
		{
			stateAction.Action = possibleActions[i];
			denominatorBoltzmann += expf(GetStateActionValue(experience, stateAction) / temperature);
		}

		//float maxStateVal = -1.0f;

		//bool found = false;

		for (int i = 0; i < (int)possibleActions.size(); i++)
		{
			const ActionRef& action = possibleActions[i];
			float stateActionValue = 0;
			stateAction.Action = possibleActions[i];
			// boost exploration
			if (updatePolicy && Configuration->BoostExploration
				&& !IsStateActionKnown(stateAction))
			{
				chosenAction = i;
				//found = true;
				ExplorationActionCount++;
				ActionChoiceCount++;
				return chosenAction;
			}
			stateActionValue = GetStateActionValue(experience, stateAction);

			// Calculate Boltzmann distribution
			float p = expf(stateActionValue / temperature) / denominatorBoltzmann;
			distribution[i] = i > 0 ? distribution[i - 1] + p : p;

			//if (!found || stateActionValue > maxStateVal)
			//{
			//	maxStateVal = stateActionValue;
			//	found = true;
			//}
			//if (found && stateActionValue > maxStateVal)
			//{
			//	chosenAction = i;
			//}
		}

		// Random number (0..1) used to select an action.
		float randomValue = RandFloat(0.0f, 1.0f);
		for (int i = 0; i < (int)possibleActions.size(); i++)
		{
			if (randomValue < distribution[i])
			{
				chosenAction = i;
				stateAction.Action = possibleActions[i];
				if (!IsStateActionKnown(stateAction))
				{
					ExplorationActionCount++;
				}
				ActionChoiceCount++;
				break;
			}
		}

		//if (chosenAction >= 0)
		//{
		//	ActionChoiceCount++;
		//}
		//else
		//{
		//	chosenAction = RandIndex((int)possibleActions.size());
		//	ExplorationActionCount++;
		//}

		return chosenAction;
	}


	int SampleAgent::GetActionChoiceCount() const
	{
		return ActionChoiceCount;
	}


	int SampleAgent::GetExplorationActionCount() const
	{
		// TODO: return -1 if you cannot provide this information
		return ExplorationActionCount;
	}


	void SampleAgent::ResetStats()
	{
		ActionChoiceCount = 0;
		ExplorationActionCount = 0;
	}


	void SampleAgent::Learn(
		const std::shared_ptr<Experience> experience,
		const std::vector<Transition>& transitionSequence,
		ActionResult lastActionResult
		)
	{
		// TODO: implement here your learning algorithm

		// you can access role datato store/retrieve action values
		// e.g.: role->GetTransitionValue(...); role->SetTransitionValue(...);

		const Transition& lastTransition = transitionSequence.back();
		std::shared_ptr<RoleInfo> role = experience->GetRole();
		EnvironmentStateInfo stateInfo = role->GetStateInfo(lastTransition.EndState);
		KnownActions.insert({ lastTransition.StartState, lastTransition.ActionTaken });

		// State reward
		float R = (float)stateInfo.Reward;
		if (!stateInfo.IsInProgress())
		{
			experience->SetStateActionValue({lastTransition.StartState, lastTransition.ActionTaken}, R);
			for (int i = (int)transitionSequence.size() - 2; i >= 0; i--)
			{
				const Transition& transition = transitionSequence[i];
				const Transition& nextTransition = transitionSequence[i + 1];
				StateActionRef stateAction{ transition.StartState, transition.ActionTaken };
				StateActionRef nextStateAction{ nextTransition.StartState, nextTransition.ActionTaken };
				if (stateInfo.Succeded())
				{
					SuccessActions.insert(nextStateAction);
				}
				if (stateInfo.Succeded() || SuccessActions.find(stateAction) == SuccessActions.end())
				{
					float nextValue = experience->GetStateActionValue(nextStateAction);
					float value = experience->GetStateActionValue(stateAction);
					const float alpha = Configuration->LearningRate;
					value = (1.0f - alpha) * value + alpha * nextValue;

					experience->SetStateActionValue(stateAction, value);
					if (!stateInfo.Succeded())
					{
						//stop here
						break;
					}
				}
			}
		}
	}


	void SampleAgent::SetConfiguration(const std::shared_ptr<IAgentConfiguration> config)
	{
		if (!config)
		{
			LogMessage(LOG_ERROR, "Null agent configuration.", "SampleAgent");
			return;
		}

		// TODO: if you are using a different configuration replace RLConfig
		if (!config->IsA("SampleAgentConfig"))
		{
			LogMessage(LOG_ERROR, "Invalid agent configuration.", "SampleAgent");
			return;
		}

		// TODO: if you are using a different configuration replace RLConfig
		Configuration = std::static_pointer_cast<SampleAgentConfig>(config);

		// TODO: if you are using a different configuration replace the following code

		// fix parameters
		//Configuration->CheckParameters();
	}


	bool SampleAgent::IsStateActionKnown(StateActionRef stateAction)
	{
		return KnownActions.find(stateAction) != KnownActions.end();
	}


	float SampleAgent::GetStateActionValue(
		const std::shared_ptr<Experience> experience,
		const discenfw::xp::StateActionRef& stateAction
		)
	{
		if (experience->StateActionValueDefined(stateAction))
		{
			return experience->GetStateActionValue(stateAction);
		}
		else
		{
			return Configuration->NewActionValue;
		}
	}


	extern "C" BOOST_SYMBOL_EXPORT SampleAgent Agent;
	SampleAgent Agent;


} // namespace sample_agent



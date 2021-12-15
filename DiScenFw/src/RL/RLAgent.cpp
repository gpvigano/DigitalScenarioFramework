//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/RL/RLAgent.h>
#include <discenfw/RL/RLConfig.h>

#include <discenfw/xp/Experience.h>

#include <discenfw/util/Rand.h>
#include <discenfw/util/MessageLog.h>

#include <iostream>
#include <math.h>

namespace discenfw
{
	// TODO: move RL classes in a new rl namespace?

	namespace xp
	{

		RLAgent::RLAgent()
		{
			QLConfiguration = std::make_shared<RLConfig>();
		}

		RLAgent::~RLAgent()
		{
		}

		void RLAgent::Reset()
		{
			Q.clear();
			StateVisitCount.clear();
			RandomActionCount = 0;
			TakenActionCount = 0;
		}




		int RLAgent::ChooseAction(
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
			bool chooseGreedy = true;
			if(updatePolicy)
			{
				/*
				Probability of taking a random action in an epsilon-greedy policy (default=0.2).

				(See Sutton&Barto 2020, p.27)
				*/
				float epsilon = GetRLConfig()->Epsilon;
				if (GetRLConfig()->EpsilonReduction > 0.0f && GetRLConfig()->EpsilonReduction < 1.0f)
				{
					int stateVisitCount = 0;
					// epsilon reduction proportional to the number of visits to this state
					// and inversely proportional to the number of choices
					stateVisitCount = StateVisitCount[envState]/(int)possibleActions.size();
					//epsilon = GetRLConfig()->Epsilon / (float)(1 + stateVisitCount);
					if (stateVisitCount > 0)
					{
						epsilon =  pow(GetRLConfig()->EpsilonReduction, stateVisitCount);
					}
					//if (epsilon < GetRLConfig()->Epsilon)
					//{
					//	epsilon = GetRLConfig()->Epsilon;
					//}
					StateVisitCount[envState]++;
				}
				float rand = RandFloat(0.0f, 1.0f);
				chooseGreedy = (epsilon < rand);
			}

			// TODO: consider implementing Upper-Confidence-Bound Action Selection
			// (See Sutton&Barto 2020, p.35)

			int chosenAction;
			if (chooseGreedy)
			{
				chosenAction = ChooseActionGreedy(possibleActions, envState);
			}
			else
			{
				RandomActionCount++;
				chosenAction = ChooseRandomAction(possibleActions);
			}
			if (chosenAction>=0)
			{
				TakenActionCount++;
			}
			return chosenAction;
		}


		int RLAgent::ChooseRandomAction(const std::vector<ActionRef>& possibleActions)
		{
			int randIndex = RandIndex((int)possibleActions.size());
			return randIndex;
		}


		int RLAgent::ChooseActionGreedy(const std::vector<ActionRef>& possibleActions, StateRef envState)
		{
			std::vector<int> maxValueActions;

			// find the maximum action value
			// and collect the actions with maximum value

			float maxStateVal = -1.0f;

			bool found = false;

			for (int i = 0; i < (int)possibleActions.size(); i++)
			{
				const ActionRef& action = possibleActions[i];
				float stateActionValue = 0;
				if (!GetStateActionValue(envState, action, stateActionValue))
				{
					// do not ignore unknown actions, they can be better than the known ones
					stateActionValue = GetRLConfig()->InitialValue;
				}
				if (!found || stateActionValue > maxStateVal)
				{
					maxStateVal = stateActionValue;
					found = true;
					maxValueActions.clear();
				}
				if (found && stateActionValue >= maxStateVal)
				{
					maxValueActions.push_back(i);
				}
			}

			if (maxValueActions.empty())
			{
				return -1;
			}
			if (maxValueActions.size() == 1)
			{
				return maxValueActions[0];
			}
			int randIndex = RandIndex((int)maxValueActions.size());

			return maxValueActions[randIndex];
		}


		int RLAgent::GetActionChoiceCount() const
		{
			return TakenActionCount;
		}


		int RLAgent::GetExplorationActionCount() const
		{
			return RandomActionCount;
		}

		void RLAgent::ResetStats()
		{
			TakenActionCount = 0;
			RandomActionCount = 0;
		}


		void RLAgent::Learn(
			const std::shared_ptr<Experience> experience,
			const std::vector<Transition>& transitionSequence,
			ActionResult lastActionResult
			)
		{
			// TODO: support for trajectory sampling
			// (See Sutton&Barto 2020, p.175)

			const Transition& lastTransition = transitionSequence.back();

			if (lastActionResult == ActionResult::SUCCEEDED)
			{
				// A successful action sequence was found,
				// backup until the initial state (all the action sequence led to the success)
				BackUp(experience, transitionSequence);
			}
			else
			{
				// In case of failure or deadlock:
				// consider the previous transition only
				// (the previous action that led to the current state)

				// Learn from the last transition
				QLearn(lastTransition, experience);
			}
		}


		void RLAgent::SetConfiguration(const std::shared_ptr<IAgentConfiguration> config)
		{
			if(!config)
			{
				LogMessage(LOG_ERROR, "Null agent configuration.", "DiScenFw");
				return;
			}

			if(!config->IsA("RLConfig"))
			{
				LogMessage(LOG_ERROR, "Invalid agent configuration.", "DiScenFw");
				return;
			}

			QLConfiguration = std::static_pointer_cast<RLConfig>(config);
			QLConfiguration->CheckParameters();

			// TODO: is it a good idea to allow changing the configuration on the run?
		}


		const std::shared_ptr<RLConfig> RLAgent::GetRLConfig() const
		{
			if (!QLConfiguration)
			{
				QLConfiguration = std::make_shared<RLConfig>();
			}
			return QLConfiguration;
		}


		bool RLAgent::GetStateActionValue(StateRef state, ActionRef action, float& value) const
		{
			const auto& Qs = Q.find(state);
			if (Qs == Q.cend())
			{
				return false;
			}
			const auto& actionValueMap = Qs->second.ActionValueMap;
			const auto& actionValue = actionValueMap.find(action);
			if (actionValue == actionValueMap.cend())
			{
				return false;
			}

			value = actionValue->second.Value;

			return true;
		}



		void RLAgent::QLearn(
			const Transition& transition,
			const std::shared_ptr<Experience> experience
			)
		{
			// Using Q-Learning reinforcement learning algorithm(see RLAgent).
			// (See Sutton&Barto 2020, p.131)

			const StateRef prevState = transition.StartState;
			const ActionRef action = transition.ActionTaken;
			const StateRef newEnvState = transition.EndState;

			std::shared_ptr<RoleInfo> role = experience->GetRole();
			EnvironmentStateInfo stateInfo = role->GetStateInfo(newEnvState);
			StateActionRef stateAction(prevState,action);

			// State reward
			float R = (float)stateInfo.Reward;

			// (state-action value,number of selections)
			auto& Q1 = Q[prevState].ActionValueMap[action];

			// state-action value for the given state-action
			float& qVal1 = Q1.Value;

			// number of selections of the given action from the given state
			int& updateCount = Q1.Count;

			updateCount++; // the first time it is 1

			if (stateInfo.IsTerminal())
			{
				qVal1 = R;
			}
			else
			{
				// if the first time then set the initial value
				// (See Sutton&Barto 2020, p.34, p.131)
				if (updateCount == 1)
				{
					qVal1 = GetRLConfig()->InitialValue;
					if (experience->StateActionValueDefined(stateAction))
					{
						qVal1 = experience->GetStateActionValue(stateAction);
					}
				}

				// find the maximum state-action value

				// estimate for the following state-action value
				float qVal2 = GetRLConfig()->InitialValue;

				if (Q.find(newEnvState) != Q.end())
				{
					// Q for (state,{action,(value,n_selections)})
					const auto& Q2 = Q.at(newEnvState);
					if (!Q2.StateVisitCountMap.empty())
					{
						// Estimate the probability to go into a state from the current one for external causes.
						// TODO: implement a MCTS algorithm (Sutton&Barto 2020, p.185)

						// total number of transitions
						int totCount = 0;
						for (auto& countPair : Q2.StateVisitCountMap)
						{
							totCount += countPair.second;
						}

						// Compute the value according to the observed transitions from the new state
						// to get an approximated distribution model.

						qVal2 = 0; // initialize accumulation (at least one value will be accumulated)
						for (auto& countPair : Q2.StateVisitCountMap)
						{
							EnvironmentStateInfo stateInfo = role->GetStateInfo(countPair.first);

							// estimate reward for the new state
							float r2 = 0;
							if(stateInfo.IsTerminal())
							{
								r2 = (float)stateInfo.Reward;
							}
							else
							{
								// get the maximum state-action value (initial value for new states)
								r2 = GetMaxValue(countPair.first);
							}
							float prob = (float)countPair.second / (float)totCount;
							qVal2 += r2 * prob;
						}
					}
					else if (!Q2.ActionValueMap.empty())
					{
						// get the maximum state-action value (initial value for new states)
						qVal2 = GetMaxValue(newEnvState);
					}

				}

				// Step-size parameter (learning rate) decreased at each update: sample-average method
				// (See Sutton&Barto 2020, p.10, p.33)
				const float sampleAverageAlpha = 1.0f / (float)updateCount;

				// Step-size parameter (learning rate)
				// if SampleAverage = true (default is false) the sample-average method is used,
				// in this way the learning rate is decreased at each update.
				// (See Sutton&Barto 2020, p.33)
				const float& alpha =
					GetRLConfig()->SampleAverage ? sampleAverageAlpha
					: GetRLConfig()->FixedStepSize;

				// Discount rate (0<=gamma<=1)
				// (See Sutton&Barto 2020, p.55)
				const float& gamma = GetRLConfig()->DiscountRate;

				// TODO: compute state emphasis based on state interest: M = I + gamma * M, alpha *= M (p.234)

				// Approximation of Bellman equation using Q-learning
				// (See Sutton&Barto 2020, p.131)
				qVal1 = (1.0f - alpha) * qVal1 + alpha * (R + gamma * qVal2);
				experience->SetStateActionValue(stateAction, qVal1);
			}
		}


		void RLAgent::BackUp(
			const std::shared_ptr<Experience> experience,
			const std::vector<Transition>& transitionSequence,
			int steps
			)
		{
			// TODO: support for trajectory sampling
			// (See Sutton&Barto 2020, p.175)

			if (steps == 0)
			{
				return;
			}
			int t = (int)transitionSequence.size();
			int first = 0;
			if (steps > 0 && steps < t)
			{
				first = t - steps;
			}

			for (int i = t - 1; i >= first; i--)
			{
				const Transition& transition = transitionSequence[i];
				QLearn(transition, experience);
			}
		}


		float RLAgent::GetMaxValue(StateRef state) const
		{
				// maximum state-action value (initial value for new states)
				float maxQ2 = GetRLConfig()->InitialValue;

				// the given state was already stored?
				bool q2Found = (Q.find(state) != Q.end());

				if (q2Found)
				{
					bool valueFound = false; // keep track of a previous value found

					// Q for (state,{action,(value,n_selections)})
					const auto& Q2 = Q.at(state);
					if (!Q2.ActionValueMap.empty())
					{
						for (const auto& actionValuePair : Q2.ActionValueMap)
						{
							// state-action value for n-th action
							const float& actionValue = actionValuePair.second.Value;

							// if it is the first found or this is greater than
							// the previous value found...
							if (!valueFound || actionValue > maxQ2)
							{
								// ...store this value
								maxQ2 = actionValue;
								valueFound = true;
							}
						}
					}
				}
			return maxQ2;
		}

	} // namespace xp

} // namespace discenfw


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
#include "discenfw/RL/IAgent.h"
#include "discenfw/RL/RLConfig.h"


namespace discenfw
{
	namespace xp
	{
		class Experience;


		/*!
		Reinforcement learning based agent.
		*/
		class DISCENFW_API RLAgent : public IAgent
		{
		public:

			RLAgent();

			virtual ~RLAgent();

			/*!
			Reset the learner, clear data.
			*/
			virtual void Reset() override;


			/*!
			Choose an action, automatically selecting a policy.

			@param experience experience (state-action values) that can be used by the agent to make a choice
			@param possibleActions list of actions that can be chosen
			@param envState current state of the environment
			@param updatePolicy update the policy according to the chosen action
			@return The index in the given array of the chosen action, -1 if no action was selected.
			*/
			virtual int ChooseAction(
				const std::shared_ptr<Experience> experience,
				const std::vector<ActionRef>& possibleActions,
				StateRef envState,
				bool updatePolicy
				) override;


			/*!
			Get the overall actions count, return 0 if no choice was made.
			*/
			virtual int GetActionChoiceCount() const override;


			/*!
			Get the exploration actions count, return 0 if no choice was made.
			*/
			virtual int GetExplorationActionCount() const override;


			/*!
			Reset statistics about the agent behavior.
			*/
			virtual void ResetStats() override;



			/*!
			Learn from a transition triggered by a chosen action.
			@param experience experience of the agent
			@param transitionSequence sequence of transitions (last one included)
			@param lastActionResult result of the last action
			*/
			virtual void Learn(
				const std::shared_ptr<Experience> experience,
				const std::vector<Transition>& transitionSequence,
				ActionResult lastActionResult
				) override;


			/*!
			Set and check the reinforcement learning configuration parameters (see RLConfig).
			*/
			virtual void SetConfiguration(const std::shared_ptr<IAgentConfiguration> config) override;


			/*!
			Get the reinforcement learning configuration parameters.
			*/
			virtual const std::shared_ptr<IAgentConfiguration> GetConfiguration() const override { return QLConfiguration; }



			/*!
			Get the Q-Learning configuration parameters (see RLConfig).
			*/
			const std::shared_ptr<RLConfig> GetRLConfig() const;


			/*!
			Get the state-action value computed by the function approximator  in Learn() method, return false if it is not available.
			*/
			virtual bool GetStateActionValue(StateRef state, ActionRef action, float& value) const;

		protected:


			/*!
			Value and number of updates for a state-action element.
			*/
			struct ValueInfo
			{
				//! Stored value
				float Value = 0.0f;

				//! Number of updates for this value.
				int Count = 0;
			};

			/*!
			Element of Q data mapping each action
			to its value and number of updates.
			*/
			struct QElem
			{
				//! Action-value mapping
				std::map< ActionRef, ValueInfo > ActionValueMap;

				//! States-updates mapping
				std::map< StateRef, int > StateVisitCountMap;
			};

			/*!
			RL data (Q-Learning), mapping each state to actions and values.
			*/
			std::map<StateRef, QElem > Q;

			/*!
			Q-Learning parameters configuration.
			*/
			mutable std::shared_ptr<RLConfig>  QLConfiguration;


			/*!
			Mapping for the number of visits to each state.
			*/
			std::map<StateRef, int > StateVisitCount;

			int RandomActionCount = 0;
			int TakenActionCount = 0;


			/*!
			Choose an action randomly.
			*/
			int ChooseRandomAction(const std::vector<ActionRef>& possibleActions);


			/*!
			Choose an action with a greedy policy (choose randomly if resulting state is unknown).
			*/
			int ChooseActionGreedy(const std::vector<ActionRef>& possibleActions, StateRef envState);


			/*!
			Update function approximator (Q-Learning).
			*/
			virtual void QLearn(
				const Transition& transition,
				const std::shared_ptr<Experience> experience
				);


			/*!
			Call QLearn() on the last transitions for back up.
			*/
			void BackUp(
				const std::shared_ptr<Experience> experience,
				const std::vector<Transition>& transitionSequence,
				int steps = -1
				);


			/*!
			Get the maximum state-action value for the given state (initial value for new states).
			*/
			float GetMaxValue(StateRef state) const;

		};
	}
}

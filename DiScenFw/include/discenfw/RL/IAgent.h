//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/xp/ref.h>
#include <discenfw/xp/RoleInfo.h>
#include <discenfw/xp/Transition.h>
#include <discenfw/xp/Action.h>
#include <discenfw/xp/EnvironmentState.h>
#include "discenfw/RL/IAgentConfiguration.h"

#include <memory>
#include <vector>
#include <map>


namespace discenfw
{
	namespace xp
	{
		class Experience;


		/*!
		Agent interface.
		*/
		class IAgent
		{
		public:

			/*!
			Reset the agent, clear data.
			*/
			virtual void Reset() = 0;

			/*!
			Choose an action, according to a policy.

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
				) = 0;


			/*!
			Get the action choice count, return 0 if no choice was made.
			*/
			virtual int GetActionChoiceCount() const = 0;


			/*!
			Get the exploration actions count, return 0 if no choice was made (return -1 if this information is not available).
			*/
			virtual int GetExplorationActionCount() const = 0;


			/*!
			Reset statistics about the agent behavior.
			*/
			virtual void ResetStats() = 0;


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
				) = 0;


			/*!
			Set and check the reinforcement learning configuration parameters (see RLConfig).
			*/
			virtual void SetConfiguration(const std::shared_ptr<IAgentConfiguration> config) = 0;

			/*!
			Get the reinforcement learning configuration parameters.
			*/
			virtual const std::shared_ptr<IAgentConfiguration> GetConfiguration() const = 0;

		};

	}
}


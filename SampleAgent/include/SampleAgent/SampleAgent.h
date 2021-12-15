//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/interop/AgentPlugin.h>

#include "SampleAgent/SampleAgentConfig.h"

#include <memory>
#include <set>

namespace sample_agent
{

	class SampleAgent : public discenfw::xp::AgentPlugin
	{
	public:

		SampleAgent();

		virtual ~SampleAgent();

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
			const std::shared_ptr<discenfw::xp::Experience> experience,
			const std::vector<discenfw::xp::ActionRef>& possibleActions,
			discenfw::xp::StateRef envState,
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
			const std::shared_ptr<discenfw::xp::Experience> experience,
			const std::vector<discenfw::xp::Transition>& transitionSequence,
			discenfw::xp::ActionResult lastActionResult
			) override;


		/*!
		Set and check the reinforcement learning configuration parameters (see RLConfig).
		*/
		virtual void SetConfiguration(const std::shared_ptr<discenfw::xp::IAgentConfiguration> config) override;


		/*!
		Get the reinforcement learning configuration parameters.
		*/
		virtual const std::shared_ptr<discenfw::xp::IAgentConfiguration> GetConfiguration() const override { return Configuration; }

	protected:

		mutable std::shared_ptr<SampleAgentConfig> Configuration;


		int ActionChoiceCount = 0;

		int ExplorationActionCount = 0;

		std::set<discenfw::xp::StateActionRef> KnownActions;
		std::set<discenfw::xp::StateActionRef> SuccessActions;


		bool IsStateActionKnown(discenfw::xp::StateActionRef stateAction);
		float GetStateActionValue(
			const std::shared_ptr<discenfw::xp::Experience> experience,
			const discenfw::xp::StateActionRef& stateAction
			);
	};

}

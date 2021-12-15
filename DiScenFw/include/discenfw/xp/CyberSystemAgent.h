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
#include "discenfw/xp/ref.h"
#include "discenfw/xp/DigitalAssistant.h"
#include "discenfw/xp/CyberSystemAssistant.h"
#include "discenfw/rl/IAgent.h"
#include "discenfw/xp/SharedArena.h"
#include "discenfw/xp/AgentStats.h"

#include <functional>


namespace discenfw
{
	namespace xp
	{
		/*!
		Agent able to act on a cyber system, to learn using a learner and to manage its experience.

		Fore each goal a new learner is created and can be trained.
		Train() method uses the inherited CyberSystemAssistant methods to interact
		with the cyber system and to update the agent experience.
		*/
		class DISCENFW_API CyberSystemAgent : public CyberSystemAssistant
		{
		public:

			/*!
			Construct an agent for the interaction with a cyber system . */
			CyberSystemAgent();

			/*!
			Construct an agent referencing an existing ICyberSystem
			with role and identifier initialization (used by the cyber system).
			*/
			CyberSystemAgent(std::shared_ptr<ICyberSystem> cyberSystem,
				const std::string& role = "", const std::string& id = "");

			virtual ~CyberSystemAgent();


			/*!
			Initialize the agent for the current goal,
			called automatically by Train().
			*/
			void Initialize();

			/*!
			Execute a single training step.
			@param updateXp update experience with DigitalAssistant
			@return the last episode state (ActionResult) or ActionResult::DEADLOCK if there is no way to go on
			@see discenfw::xp::ActionResult, ActionResult::DEADLOCK
			*/
			ActionResult Train(bool updateXp);


			/*!
			Take an action, let the agent process its result.
			@param actionChosen action to be taken.
			@param updateXp update experience with DigitalAssistant
			@return the last episode state (ActionResult) or ActionResult::DEADLOCK if there is no way to go on
			@see discenfw::xp::ActionResult, ActionResult::DEADLOCK
			*/
			virtual ActionResult TakeAction(const Action& actionChosen, bool updateXp = true) override;


			/*!
			Notify the last result to the agent, if it is a consequence of external actions.
			*/
			virtual void UpdateState() override;

			/*!
			Enable or disable deadlock detection (enabled by default).
			*/
			void SetLoopDetectionEnabled(bool enabled) { LoopDetectionEnabled = enabled; }

			/*!
			Check if deadlock detection is enabled (enabled by default).
			*/
			bool IsLoopDetectionEnabled() { return LoopDetectionEnabled; }


			/*!
			Enable or disable learning (enabled by default).
			*/
			void SetLearningEnabled(bool enabled) { LearningEnabled = enabled; }

			/*!
			Check if learning is enabled (enabled by default).
			*/
			bool IsLearningEnabled() { return LearningEnabled; }


			/*!
			Get the reinforcement learning configuration parameters (see RLConfig).
			*/
			const std::shared_ptr<IAgentConfiguration> GetAgentConfiguration();

			/*!
			Set and check the reinforcement learning configuration parameters (see RLConfig).
			*/
			void SetAgentConfiguration(const std::shared_ptr<IAgentConfiguration> config);


			/*!
			Get statistics about the agent behavior.
			*/
			AgentStats GetStatistics() const { return Statistics; }

			/*!
			Reset statistics about the agent behavior.
			*/
			void ResetStats();

			/*!
			Completely reset the agent.
			*/
			void ResetAgentForCurrentGoal();

			/*!
			Completely reset all the agents.
			*/
			void ResetAgent();

			/*!
			Check if a new episode was started automatically after a call to Train().
			*/
			bool IsNewEpisode() { return NewEpisodeStarted; }


			/*!
			Replace the default RL learner creation with a callback function uesd to create custom learners.
			@note All previous learners and their related data are destroyed after this call.
			*/
			void SetCustomAgentMaker(std::function<std::shared_ptr<IAgent>()> makeAgentCallback);

		protected:

			/*!
			Current Reinforcement Learning parameters configuration.
			*/
			std::shared_ptr<IAgentConfiguration> AgentConfiguration;

			/*!
			Optional callback function uesd to create custom learners.
			*/
			std::function<std::shared_ptr<IAgent>()> CustomAgentMaker;

			/*!
			Mapping from goals to Agents: one agent for each goal.
			*/
			std::map< std::string, std::shared_ptr<IAgent> > Agents;

			/*!
			This keeps track of the last result, used by Train().
			*/
			ActionResult LastActionResult = ActionResult::IN_PROGRESS;

			bool NewEpisodeStarted = false;

			/*!
			Flag used to enable deadlock detection in Learners.
			*/
			bool LoopDetectionEnabled = true;

			/*!
			Flag used to enable learning.
			*/
			bool LearningEnabled = true;


			/*!
			Mapping for the number of visits to each state.
			*/
			std::map<StateRef, int > StateVisitCount;


			/*!
			Set of visited states, used for statistics.
			*/
			std::set< StateRef > StateSet;

			/*!
			Last action/destination-state, used for RL.
			*/
			Transition LastTransition;

			/*!
			Mapping from states and actions that lead to a deadlock.
			*/
			std::map< StateRef, std::vector<ActionRef> > DeadlockActions;

			/*!
			Statistics computed while learning.
			*/
			AgentStats Statistics;


			/*!
			Create a agent and return the agent shared pointer.
			*/
			virtual std::shared_ptr<IAgent> MakeAgent();


			/*!
			Get a reference for the learner for the current goal, create it if not defined.
			*/
			std::shared_ptr<IAgent> GetAgent();


			/*!
			Initilize the agent the first time, start a new episode if the current one is complete.
			*/
			void ProcessLastEpisode();


			/*!
			Choose and take actions, then learn from results.
			@param updateXp update experience with DigitalAssistant.
			@param learn let this agent learn (default is true).
			@return Return the action result or (DEADLOCK if no further action can be taken).
			*/
			ActionResult Act(
				bool updateXp
				);


			/*!
			Update the last action result from its consequences, then learn from results.
			*/
			void UpdateLastAction();



			/*!
			Discard available actions that bring to a previous state to avoid infinite loops.
			*/
			bool GetPossibleActions(
				const StateRef prevStateRef,
				const std::vector<ActionRef>& availableActions,
				std::vector<ActionRef>& possibleActions
				);


			/*!
			Detect actions that bring to a previous state, to avoid infinite loops.
			*/
			bool DetectLoop(const std::vector<Transition>& transitionSequence, const Transition& transition);


			/*!
			Process the result of an action.
			*/
			ActionResult ProcessActionResult(
				StateRef prevStateRef,
				const ActionRef actionChosen,
				ActionResult actionChosenResult,
				int alternativeActionCount,
				bool updateXp
				);


			/*!
			Update statistics based on the given action result.
			*/
			void UpdateStats(ActionResult result);


			/*!
			Store the given state reference and update the states counter.
			*/
			StateRef RegisterState(std::shared_ptr<EnvironmentState> state);
		};
	}
}


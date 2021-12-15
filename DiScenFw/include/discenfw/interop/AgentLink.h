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


#include <memory>
#include <functional>

/// @cond DOXYGEN_IGNORE
namespace boost { template <class T> class shared_ptr; }
/// @endcond

namespace discenfw
{
	namespace xp
	{
		class Experience;
		class AgentPlugin;

		/*!
		Binding of IAgent methods to an external DLL (AgentPlugin).

		It provides methods to load an agent implementation at run-time and
		reroute interface methods calls to that implementation.
		@see IAgent, AgentPlugin
		*/
		class DISCENFW_API AgentLink : public IAgent
		{
		public:
			AgentLink();
			virtual ~AgentLink();

			/*!
			Reset the agent, clear data.
			*/
			virtual void Reset();

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
				);


			/*!
			Get the action choice count, return 0 if no choice was made.
			*/
			virtual int GetActionChoiceCount() const;


			/*!
			Get the exploration actions count, return 0 if no choice was made (return -1 if this information is not available).
			*/
			virtual int GetExplorationActionCount() const;


			/*!
			Reset statistics about the agent behavior.
			*/
			virtual void ResetStats();


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
				);


			/*!
			Set and check the reinforcement learning configuration parameters (see RLConfig).
			*/
			virtual void SetConfiguration(const std::shared_ptr<IAgentConfiguration> config);

			/*!
			Get the reinforcement learning configuration parameters.
			*/
			virtual const std::shared_ptr<IAgentConfiguration> GetConfiguration() const;



			/*!
			Load an agent implementation with the given name.
			@param agentPluginName Path without extension (-x32 or -x64 are automatically added under Windows platform).
			@return true if successfully loaded, false otherwise.
			*/
			bool LoadAgentPlugin(const std::string& agentPluginName);

			/*!
			Check if an agent implementation is loaded.
			*/
			bool IsAgentPluginLoaded() const;

			/*!
			Check if an agent implementation is loaded, log an error if not.
			*/
			bool CheckAgentPluginLoaded() const;

			/*!
			Check if the given agent implementation is loaded.
			*/
			bool IsAgentPluginLoaded(const std::string& agentPluginName) const;

			/*!
			Get the name of the loaded agent implementation (the name used to load it).
			*/
			const std::string& GetAgentPluginName() const { return AgentPluginName; }

		protected:
			std::string AgentPluginName;

			std::unique_ptr < boost::shared_ptr<AgentPlugin> > PluginPtr;
		};
	}
}

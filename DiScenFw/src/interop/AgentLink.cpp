//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/interop/AgentLink.h>
#include <discenfw/util/MessageLog.h>
#include <discenfw/interop/AgentPlugin.h>

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <boost/dll/import.hpp> // for import_alias
#include <boost/dll/runtime_symbol_info.hpp> // for program_location
#include <boost/smart_ptr/shared_ptr.hpp>

#include <iostream>
#include <memory>

namespace discenfw
{
	namespace xp
	{
		/*
		Sample agent plugin implementation:

		in MyAgentPlugin.h:

		#include <discenfw/xp/AgentPlugin.h>
		class MyAgentPlugin : public AgentPlugin

		in MyAgentPlugin.cpp:

		namespace myAgent
		{
		// Exporting `myAgent::Agent` variable with alias name `MyAgent`
		// (Has the same effect as `BOOST_DLL_ALIAS(myAgent::Agent, Agent)`)
		extern "C" BOOST_SYMBOL_EXPORT MyAgentPlugin Agent;
		MyAgentPlugin Agent;
		}
		*/

		AgentLink::AgentLink()
		{
		}


		AgentLink::~AgentLink()
		{
		}


		void AgentLink::Reset()
		{
			if (!CheckAgentPluginLoaded())
			{
				return;
			}
			return (*PluginPtr)->Reset();
		}


		int AgentLink::ChooseAction(
			const std::shared_ptr<Experience> experience,
			const std::vector<ActionRef>& possibleActions,
			StateRef envState,
			bool updatePolicy
			)
		{
			if (!CheckAgentPluginLoaded())
			{
				return -1;
			}
			return (*PluginPtr)->ChooseAction(experience, possibleActions, envState, updatePolicy);
		}


		int AgentLink::GetActionChoiceCount() const
		{
			if (!CheckAgentPluginLoaded())
			{
				return -1;
			}
			return (*PluginPtr)->GetActionChoiceCount();
		}


		int AgentLink::GetExplorationActionCount() const
		{
			if (!CheckAgentPluginLoaded())
			{
				return -1;
			}
			return (*PluginPtr)->GetExplorationActionCount();
		}


		void AgentLink::ResetStats()
		{
			if (!CheckAgentPluginLoaded())
			{
				return;
			}
			return (*PluginPtr)->ResetStats();
		}


		void AgentLink::Learn(
			const std::shared_ptr<Experience> experience,
			const std::vector<Transition>& transitionSequence,
			ActionResult lastActionResult)
		{
			if (!CheckAgentPluginLoaded())
			{
				return;
			}
			return (*PluginPtr)->Learn(experience, transitionSequence, lastActionResult);
		}


		void AgentLink::SetConfiguration(const std::shared_ptr<IAgentConfiguration> config)
		{
			if (!CheckAgentPluginLoaded())
			{
				return;
			}
			return (*PluginPtr)->SetConfiguration(config);
		}


		const std::shared_ptr<IAgentConfiguration> AgentLink::GetConfiguration() const
		{
			if (!CheckAgentPluginLoaded())
			{
				return nullptr;
			}
			return (*PluginPtr)->GetConfiguration();
		}


		bool AgentLink::LoadAgentPlugin(const std::string& agentPluginName)
		{
			if (PluginPtr && agentPluginName == AgentPluginName)
			{
				LogMessage(LOG_DEBUG, "Plugin " + agentPluginName + " already loaded.", "DiScenFw");
				return true;
			}
			std::string completeName = agentPluginName;
			// TODO: manage architecture also for other operating systems
#if defined(_WIN32)
			if (agentPluginName.size() > 4)
			{
				std::string suffix = agentPluginName.substr(agentPluginName.size() - 4);
#if defined(_WIN64)
				if (suffix != "-x64")
				{
					completeName += "-x64";
				}
#else
				if (suffix != "-x86")
				{
					completeName += "-x86";
				}
#endif
			}
#endif

			//boost::dll::fs::path lib_path(boost::dll::program_location()); // path to the executable
			//lib_path.remove_filename();
			//lib_path /= "plugins";
			//lib_path /= name; // path to the library and library name
			boost::dll::fs::path lib_path(completeName); // path to the library
			LogMessage(LOG_DEBUG, "Loading the plugin " + completeName, "DiScenFw");
			boost::shared_ptr<AgentPlugin> loadedPlugin;
			std::string errStr;
			try
			{
				// variable to hold a pointer to plugin variable
				// type of imported symbol is located between `<` and `>`
				loadedPlugin = boost::dll::import<AgentPlugin>(
					lib_path, // path to the library and library name
					"Agent",  // name of the symbol to import
					boost::dll::load_mode::append_decorations // makes `MyAgent.so` or `MyAgent.dll` from `MyAgent`
					);
			}
			catch (boost::dll::fs::system_error e)
			{
				errStr = e.what();
			}
			if (loadedPlugin)
			{
				LogMessage(LOG_DEBUG, "Plugin " + agentPluginName + " loaded", "DiScenFw");
			}
			else
			{
				LogMessage(LOG_ERROR, "Failed to load the agent plugin " + agentPluginName + ": " + errStr, "DiScenFw");
			}
			if (!loadedPlugin)
			{
				return false;
			}

			PluginPtr = std::make_unique< boost::shared_ptr<AgentPlugin> >(loadedPlugin);
			AgentPluginName = agentPluginName;

			return true;
		}


		bool AgentLink::IsAgentPluginLoaded() const
		{
			return PluginPtr != nullptr;
		}


		bool AgentLink::CheckAgentPluginLoaded() const
		{
			if (!PluginPtr)
			{
				LogMessage(LOG_ERROR, "No agent plugin loaded.", "DiScenFw", true, true, "AgentPluginFail");
				return false;
			}
			return true;
		}


		bool AgentLink::IsAgentPluginLoaded(const std::string& agentPluginName) const
		{
			return PluginPtr != nullptr && AgentPluginName == agentPluginName;
		}

	} // namespace xp
} // namespace discenfw


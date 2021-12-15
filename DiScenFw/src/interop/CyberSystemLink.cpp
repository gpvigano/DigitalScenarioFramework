//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/interop/CyberSystemLink.h>
#include <discenfw/util/MessageLog.h>
#include <discenfw/interop/CyberSystemPlugin.h>

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <boost/dll/import.hpp> // for import_alias
#include <boost/dll/runtime_symbol_info.hpp> // for program_location
#include <boost/smart_ptr/shared_ptr.hpp>

#include <iostream>

namespace discenfw
{
	namespace xp
	{
		/*
		in MyCyberSystemPlugin.h:

		#include <discenfw/interop/CyberSystemPlugin.h>
		class MyCyberSystemPlugin : public CyberSystemPlugin

		in MyCyberSystemPlugin.cpp:

		namespace mycybersystem
		{
		// Exporting `mycybersystem::CyberSystem` variable with alias name `MyCyberSystem`
		// (Has the same effect as `BOOST_DLL_ALIAS(mycybersystem::CyberSystem, CyberSystem)`)
		extern "C" BOOST_SYMBOL_EXPORT MyCyberSystemPlugin CyberSystem;
		MyCyberSystemPlugin CyberSystem;
		}
		*/

		CyberSystemLink::CyberSystemLink()
		{
		}


		CyberSystemLink::~CyberSystemLink()
		{
		}


		void CyberSystemLink::SynchronizeState(std::shared_ptr<EnvironmentState> environmentState)
		{
			if (CheckCyberSystemLoaded())
			{
				(*PluginPtr)->SynchronizeState(environmentState);
			}
		}


		const std::vector<ActionRef>& CyberSystemLink::GetAvailableActions(
			const std::string& roleId,
			bool smartSelection
			) const
		{
			if (!CheckCyberSystemLoaded())
			{
				static std::vector<xp::ActionRef> emptyActionVector;
				return emptyActionVector;
			}
			return (*PluginPtr)->GetAvailableActions(roleId, smartSelection);
		}


		void CyberSystemLink::InitFailureConditions()
		{
			if (CheckCyberSystemLoaded())
			{
				(*PluginPtr)->InitFailureConditions();
			}
		}


		void CyberSystemLink::InitRoles()
		{
			if (!CheckCyberSystemLoaded())
			{
				return;
			}
			return (*PluginPtr)->InitRoles();
		}


		bool CyberSystemLink::ExecuteAction(const Action& action)
		{
			if (!CheckCyberSystemLoaded())
			{
				return false;
			}
			return (*PluginPtr)->ExecuteAction(action);
		}


		const EnvironmentState& CyberSystemLink::InterpretSystemState()
		{
			if (PluginPtr)
			{
				return (*PluginPtr)->InterpretSystemState();
			}
			static EnvironmentState nullState;
			return nullState;
		}


		void CyberSystemLink::ResetSystem()
		{
			if (PluginPtr)
			{
				(*PluginPtr)->ResetSystem();
			}
		}


		const std::string CyberSystemLink::GetSystemName() const
		{
			if (!CheckCyberSystemLoaded())
			{
				return std::string();
			}
			return  (*PluginPtr)->GetSystemName();
		}


		const std::string CyberSystemLink::GetSystemInfo(const std::string& infoId) const
		{
			if (!CheckCyberSystemLoaded())
			{
				return std::string();
			}
			return (*PluginPtr)->GetSystemInfo(infoId);
		}


		bool CyberSystemLink::SetConfiguration(const std::string& config)
		{
			if (!CheckCyberSystemLoaded())
			{
				return false;
			}
			return (*PluginPtr)->SetConfiguration(config);
		}


		const std::string CyberSystemLink::GetConfiguration()
		{
			if (!CheckCyberSystemLoaded())
			{
				return std::string();
			}
			return (*PluginPtr)->GetConfiguration();
		}


		const std::string CyberSystemLink::ReadEntityConfiguration(const std::string& entityId)
		{
			if (!CheckCyberSystemLoaded())
			{
				return std::string();
			}
			return (*PluginPtr)->ReadEntityConfiguration(entityId);
		}


		bool CyberSystemLink::WriteEntityConfiguration(const std::string& entityId, const std::string& config)
		{
			if (!CheckCyberSystemLoaded())
			{
				return false;
			}
			return (*PluginPtr)->WriteEntityConfiguration(entityId, config);
		}

		bool CyberSystemLink::ConfigureEntity(const std::string& entityId, const std::string& entityType, const std::string& config)
		{
			if (!CheckCyberSystemLoaded())
			{
				return false;
			}
			return (*PluginPtr)->ConfigureEntity(entityId, entityType, config);
		}

		bool CyberSystemLink::RemoveEntity(const std::string& entityId)
		{
			if (!CheckCyberSystemLoaded())
			{
				return false;
			}
			return (*PluginPtr)->RemoveEntity(entityId);
		}


		void CyberSystemLink::Initialize(bool rebuild)
		{
			if (CheckCyberSystemLoaded())
			{
				return (*PluginPtr)->Initialize(rebuild);
			}
		}


		bool CyberSystemLink::IsInitialized() const
		{
			if (!CheckCyberSystemLoaded())
			{
				return false;
			}
			return (*PluginPtr)->IsInitialized();
		}


		void CyberSystemLink::Clear()
		{
			if (CheckCyberSystemLoaded())
			{
				return (*PluginPtr)->Clear();
			}
		}


		const EnvironmentState& CyberSystemLink::GetInitialSystemState()
		{
			if (!CheckCyberSystemLoaded())
			{
				static EnvironmentState nullState;
				return nullState;
			}
			return (*PluginPtr)->GetInitialSystemState();
		}


		const EnvironmentState& CyberSystemLink::GetLastSystemState()
		{
			if (!CheckCyberSystemLoaded())
			{
				static EnvironmentState nullState;
				return nullState;
			}
			return (*PluginPtr)->GetLastSystemState();
		}


		const Condition& CyberSystemLink::GetFailureCondition() const
		{
			if (!CheckCyberSystemLoaded())
			{
				static Condition nullCondition;
				return nullCondition;
			}
			return (*PluginPtr)->GetFailureCondition();
		}


		void CyberSystemLink::SetLogEnabled(bool enabled)
		{
			if (!CheckCyberSystemLoaded())
			{
				return;
			}
			(*PluginPtr)->SetLogEnabled(enabled);
		}


		bool CyberSystemLink::IsLogEnabled() const
		{
			if (!CheckCyberSystemLoaded())
			{
				return false;
			}
			return (*PluginPtr)->IsLogEnabled();
		}


		const std::shared_ptr<EnvironmentModel> CyberSystemLink::GetModel() const
		{
			if (!CheckCyberSystemLoaded())
			{
				return nullptr;
			}
			return xp::GetModel((*PluginPtr)->GetSystemName());
		}


		bool CyberSystemLink::LoadCyberSystem(const std::string& cyberSystemPluginName)
		{
			if (PluginPtr && cyberSystemPluginName == CyberSystemPluginName)
			{
				LogMessage(LOG_DEBUG, "Plugin " + cyberSystemPluginName + " already loaded.", "DiScenFw");
				return true;
			}
			std::string completeName = cyberSystemPluginName;
			// TODO: manage architecture also for other operating systems
#ifdef _WIN32
			if (cyberSystemPluginName.size() > 4)
			{
				std::string suffix = cyberSystemPluginName.substr(cyberSystemPluginName.size() - 4);
#ifdef _WIN64
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
			boost::shared_ptr<CyberSystemPlugin> loadedPlugin;
			std::string errStr;
			try
			{
				// variable to hold a pointer to plugin variable
				// type of imported symbol is located between `<` and `>`
				loadedPlugin = boost::dll::import<CyberSystemPlugin>(
					lib_path, // path to the library and library name
					"CyberSystem",  // name of the symbol to import
					boost::dll::load_mode::append_decorations // makes `MyCyberSystem.so` or `MyCyberSystem.dll` from `MyCyberSystem`
					);
			}
			catch (boost::dll::fs::system_error e)
			{
				errStr = e.what();
			}
			if (loadedPlugin)
			{
				LogMessage(LOG_DEBUG, "Plugin " + cyberSystemPluginName + " loaded", "DiScenFw");
			}
			else
			{
				LogMessage(LOG_ERROR, "Failed to load the cyber system plugin " + cyberSystemPluginName + ": " + errStr, "DiScenFw");
			}
			if (!loadedPlugin)
			{
				return false;
			}
			PluginPtr = std::make_unique< boost::shared_ptr<CyberSystemPlugin> >(loadedPlugin);
			CyberSystemPluginName = cyberSystemPluginName;

			return true;
		}


		bool CyberSystemLink::IsCyberSystemLoaded() const
		{
			return PluginPtr != nullptr;
		}


		bool CyberSystemLink::CheckCyberSystemLoaded() const
		{
			if (!PluginPtr)
			{
				LogMessage(LOG_ERROR, "No cyber system plugin loaded.", "DiScenFw", true, true, "CybSysFail");
				return false;
			}
			return true;
		}


		bool CyberSystemLink::IsCyberSystemLoaded(const std::string& cyberSystemPluginName) const
		{
			return PluginPtr != nullptr && CyberSystemPluginName == cyberSystemPluginName;
		}

	} // namespace xp
} // namespace discenfw


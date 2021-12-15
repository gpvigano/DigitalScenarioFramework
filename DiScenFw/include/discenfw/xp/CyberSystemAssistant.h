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
#include "discenfw/xp/DigitalAssistant.h"
#include "discenfw/xp/ICyberSystem.h"
#include <memory>

namespace discenfw
{
	/*!
	Experience management
	*/
	namespace xp
	{
		class SharedArena;

		/*!
		A DigitalAssistant interfaced with a cyber system.
		The assistant interacts with the cyber system and updates the experience.
		*/
		class DISCENFW_API CyberSystemAssistant : public DigitalAssistant, public std::enable_shared_from_this<CyberSystemAssistant>
		{
		public:

			/*!
			Default empty constructor.
			*/
			CyberSystemAssistant();

			/*!
			Construct an assistant referencing an existing ICyberSystem
			with role and identifier initialization (used by the cyber system).
			*/
			CyberSystemAssistant(std::shared_ptr<ICyberSystem> cyberSystem,
				const std::string& role = "", const std::string& id = "");

			virtual ~CyberSystemAssistant();

			/*!
			Connect the assistant to an existing ICyberSystem.
			@param cyberSystem system to be linked.
			@param role optional role initialization.
			@param id optional identifier initialization.
			*/
			void SetCyberSystem(std::shared_ptr<ICyberSystem> cyberSystem,
				const std::string& role = "", const std::string& id = "");

			/*!
			Get the connected ICyberSystem or nullptr if not connected.
			*/
			std::shared_ptr<ICyberSystem> GetCyberSystem() { return CyberSystem; }


			/*!
			Add this assistant to a SharedArena.
			*/
			void SetArena(const std::shared_ptr<SharedArena>& sharedArena, bool add = true);


			/*!
			Set the current goal: experience is stored separately for each goal.
			*/
			bool SetCurrentGoal(const std::string& goal) override;

			/*!
			Start a new episode with the stored initial state (parameter is ignored).
			*/
			bool StartEpisode(const std::shared_ptr<EnvironmentState> initialState) override;

			/*!
			Start a new episode with the initial state (retrieved from the system at the first call).
			*/
			bool NewEpisode() override;

			virtual EnvironmentStateInfo GetStateInfo(const std::shared_ptr<EnvironmentState> environmentState) const;

			/*!
			Call CarryOutAction(), InterpretSystemState() and store the action (Assistant).
			@param action action to be taken
			@param update experience (default is true)
			*/
			virtual ActionResult TakeAction(const Action& action, bool updateXp = true);


			/*!
			Shorcut to the current experience, create it if not found for the current goal.
			*/
			virtual std::shared_ptr<Experience> CurrentExperience() override;


			/*!
			Check if the cyber system is initialized.
			*/
			bool IsInitialized();

			/*!
			Initialize the cyber system if not yet initialized.
			*/
			void Initialize();

			/*!
			Get a list of available actions for the current state of the system.
			*/
			const std::vector<ActionRef>& GetAvailableActions(bool smartSelection = false) const;

			/*!
			Get information from the system.
			*/
			std::string GetSystemInfo(const std::string& infoId = "") const;

			/*!
			Set the cyber system configuration encoded in a text string.
			*/
			bool SetSystemConfiguration(const std::string& config);

			/*!
			Get the cyber system configuration encoded in a text string.
			*/
			std::string GetSystemConfiguration();


			virtual void SetLogEnabled(bool enabled) override;


			/*!
			Get the action reference stored in the model corresponding to the given one.
			*/
			ActionRef GetActionRef(const Action& action);

		protected:

			/*!
			Internal reference to the digital assistant for experience management.
			*/
			std::shared_ptr<ICyberSystem> CyberSystem;


			/*!
			Internal reference to a SharedArena where CyberSystemAssistants
			interact with the digital scenario.
			*/
			std::weak_ptr<SharedArena> Arena;
		};



		inline bool CyberSystemAssistant::IsInitialized()
		{
			return CyberSystem->IsInitialized();
		}


		inline const std::vector<ActionRef>& CyberSystemAssistant::GetAvailableActions(
			bool smartSelection
			) const
		{
			return CyberSystem->GetAvailableActions(CurrentRole, smartSelection);
		}


		inline std::string CyberSystemAssistant::GetSystemInfo(const std::string& infoId) const
		{
			return CyberSystem->GetSystemInfo(infoId);
		}


		inline bool CyberSystemAssistant::SetSystemConfiguration(const std::string& config)
		{
			return CyberSystem->SetConfiguration(config);
		}


		inline std::string CyberSystemAssistant::GetSystemConfiguration()
		{
			return CyberSystem->GetConfiguration();
		}


		inline void CyberSystemAssistant::SetLogEnabled(bool enabled)
		{
			DigitalAssistant::SetLogEnabled(enabled);
			CyberSystem->SetLogEnabled(enabled);
		}

		inline ActionRef CyberSystemAssistant::GetActionRef(const Action& action)
		{
			return GetCurrentExperience()->GetModel()->EncodeAction(action);
		}

	}
}


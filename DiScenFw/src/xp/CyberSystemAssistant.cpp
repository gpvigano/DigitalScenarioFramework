//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/CyberSystemAssistant.h>
#include <discenfw/xp/EnvironmentModel.h>
#include <discenfw/xp/SharedArena.h>

#include <fstream>
#include <iostream>

namespace discenfw
{
	namespace xp
	{

		CyberSystemAssistant::CyberSystemAssistant()
		{
		}

		CyberSystemAssistant::CyberSystemAssistant(
			std::shared_ptr<ICyberSystem> cyberSystem,
			const std::string& role,
			const std::string& id) :
				DigitalAssistant(role, id),
				CyberSystem(cyberSystem)
		{
		}


		CyberSystemAssistant::~CyberSystemAssistant()
		{
		}



		void CyberSystemAssistant::SetCyberSystem(
			std::shared_ptr<ICyberSystem> cyberSystem,
			const std::string& role,
			const std::string& id)
		{
			CyberSystem = cyberSystem;
			CurrentRole = role;
			Id = id;
		}


		void CyberSystemAssistant::SetArena(const std::shared_ptr<SharedArena>& sharedArena, bool add)
		{
			Arena = sharedArena;
			if (add && sharedArena && sharedArena->HasActor(shared_from_this()))
			{
				sharedArena->Actors.push_back(shared_from_this());
			}
		}


		bool CyberSystemAssistant::SetCurrentGoal(const std::string& goal)
		{
			if (!DigitalAssistant::SetCurrentGoal(goal))
			{
				return false;
			}
			if (CyberSystem && !CurrentRole.empty())
			{
				CyberSystem->InitRoles();
			}
			return true;
		}


		bool CyberSystemAssistant::StartEpisode(const std::shared_ptr<EnvironmentState> initialState)
		{
			if (!initialState)
			{
				return NewEpisode();
			}
			return DigitalAssistant::StartEpisode(initialState);
			//return NewEpisode();
		}


		void CyberSystemAssistant::Initialize()
		{
			if (!CyberSystem->IsInitialized() ||
				CurrentExperience()->GetModel()->IsDefined())
			{
				CyberSystem->Initialize(true);
				CyberSystem->InitRoles();
			}
		}


		bool CyberSystemAssistant::NewEpisode()
		{
			CyberSystem->ResetSystem();
			CyberSystem->Initialize();
			const EnvironmentState& systemState = CyberSystem->GetInitialSystemState();
			const std::shared_ptr<EnvironmentState> initState = CurrentExperience()->GetStoredState(systemState);
			DigitalAssistant::StartEpisode(initState);
			return true;
		}


		EnvironmentStateInfo CyberSystemAssistant::GetStateInfo(const std::shared_ptr<EnvironmentState> environmentState) const
		{
			if (!CyberSystem->IsInitialized())
			{
				CyberSystem->Initialize(true);
			}
			std::shared_ptr<RoleInfo> role = GetRole();
			if (!role)
			{
				CyberSystem->InitRoles();
				role = GetRole();
			}
			if (!GetCurrentExperience()->SystemFailureIgnored && CyberSystem)
			{
				const Condition& failureCondition = CyberSystem->GetFailureCondition();
				if (failureCondition.Defined())
				{
					if (failureCondition.Evaluate(environmentState))
					{
						return role->OverrideStateResult(environmentState, ActionResult::FAILED);
					}
				}
			}

			return role->GetStateInfo(environmentState);
		}


		ActionResult CyberSystemAssistant::TakeAction(const Action& action, bool updateXp)
		{
			StateRef lastState = GetLastState();
			if (!lastState)
			{
				const EnvironmentState& lastSystemState = CyberSystem->InterpretSystemState();
				const std::shared_ptr<EnvironmentState> currentState = CurrentExperience()->GetStoredState(lastSystemState);
				StartEpisode(currentState);
			}

			if (!CyberSystem->ExecuteAction(action))
			{
				return ActionResult::DENIED;
			}
			const EnvironmentState& systemState = CyberSystem->InterpretSystemState();
			const std::shared_ptr<EnvironmentState> newState = CurrentExperience()->GetStoredState(systemState);

			ActionResult result = ProcessAction(action, newState, updateXp);

			return result;
		}


		std::shared_ptr<Experience> CyberSystemAssistant::CurrentExperience()
		{
			if (CurrentModel.empty())
			{
				CurrentModel = CyberSystem->GetSystemName();
			}
			return DigitalAssistant::CurrentExperience();
		}

	} //namespace discenfw

} //namespace xp


//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <DiScenFw/DigitalScenarioFramework.h>

#include <discenfw/scen/ScenarioData.h>
#include <discenfw/scen/ScenarioManager.h>
#include <discenfw/scen/Catalog.h>

#include <discenfw/ve/VirtualEnvironmentAPI.h>

#include <discenfw/sim/SimulationManager.h>

#include <discenfw/xp/SharedArena.h>
#include <discenfw/xp/ExperienceLevel.h>
#include <discenfw/xp/CyberSystemAgent.h>
#include <discenfw/interop/CyberSystemLink.h>
#include <discenfw/xp/SharedArena.h>
#include <DiScenFw/xp/CyberSystemAgent.h>
#include <discenfw/xp/EnvironmentModel.h>

#include <discenfw/util/MessageLog.h>

#include <gpvulc/text/text_util.h>
#include <gpvulc/json/RapidJsonInclude.h> // ParseException, FormatException

#include <boost/filesystem.hpp>

#include "JSON/JsonRLConfig.h"
#include "JSON/JsonCatalog.h"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <locale>

namespace discenfw
{
	using namespace xp;

	// Static members

	std::shared_ptr<DigitalScenarioFramework> DigitalScenarioFramework::Instance;

	std::string DigitalScenarioFramework::EmptyString;
	std::vector<std::string> DigitalScenarioFramework::EmptyStringVector;
	std::vector<Action> DigitalScenarioFramework::EmptyActionVector;
	std::vector<ActionRef> DigitalScenarioFramework::EmptyActionRefVector;
	Condition DigitalScenarioFramework::EmptyCondition;
	StateRewardRules DigitalScenarioFramework::EmptyStateReward;




	std::shared_ptr<DigitalScenarioFramework> DigitalScenarioFramework::GetInstance()
	{
		if (!Instance)
		{
			Instance = std::shared_ptr<DigitalScenarioFramework>(new DigitalScenarioFramework());
		}
		return Instance;
	}


	DigitalScenarioFramework::DigitalScenarioFramework()
	{
	}


	DigitalScenarioFramework::~DigitalScenarioFramework()
	{
	}


	bool DigitalScenarioFramework::LoadCyberSystem(const std::string& cyberSystemName)
	{
		InitCyberSystem();
		if (!CyberSystem->LoadCyberSystem(cyberSystemName))
		{
			return false;
		}
		CyberSystem->Initialize();
		return true;
	}


	bool DigitalScenarioFramework::IsCyberSystemLoaded(const std::string& cyberSystemName)
	{
		InitCyberSystem();
		return CyberSystem->IsCyberSystemLoaded(cyberSystemName);
	}


	bool DigitalScenarioFramework::IsCyberSystemLoaded()
	{
		InitCyberSystem();
		return CyberSystem->IsCyberSystemLoaded();
	}


	const std::string& DigitalScenarioFramework::GetSystemConfiguration()
	{
		if (!CheckCyberSystem())
		{
			LastSystemConfig = "";
		}
		else
		{
			LastSystemConfig = CyberSystem->GetConfiguration();
		}
		return LastSystemConfig;
	}


	bool DigitalScenarioFramework::SetSystemConfiguration(const std::string& configText)
	{
		if (!CheckCyberSystem())
		{
			return false;
		}
		if (!CyberSystem->SetConfiguration(configText))
		{
			return false;
		}
		return SaveSystemConfigurationToScenario();
	}


	bool DigitalScenarioFramework::ConfigureEntity(const std::string& entityId, const std::string& configText)
	{
		if (!CheckCyberSystem())
		{
			return false;
		}
		InitDigitalScenario();
		const std::shared_ptr< Entity > entity = DigitalScenarioManager->GetEntityById(entityId);
		if (!entity)
		{
			LogMessage(LOG_ERROR, "Entity '" + entityId + "' not found, cannot configure it.", "DiScenFw", true, true, "ENTITY_NOT_FOUND");
			return false;
		}

		if (!CyberSystem->ConfigureEntity(entityId, entity->Identity.Type, configText))
		{
			LogMessage(LOG_ERROR, "Failed to set configuration for entity " + entity->Identity.Identifier, "DiScenFw");
			return false;
		}
		entity->Configuration = configText;
		return true;
	}

	bool DigitalScenarioFramework::ConfigureSystemFromScenario()
	{
		if (!CheckCyberSystem())
		{
			return false;
		}
		CyberSystem->ResetSystem();
		InitDigitalScenario();
		CyberSystem->Clear();
		CyberSystem->Initialize(true);
		const std::vector< std::shared_ptr<Entity> >& entities = ScenarioData()->Entities;
		bool ok = true;
		for (const auto& entity : entities)
		{
			if (!CyberSystem->ConfigureEntity(entity->Identity.Identifier, entity->Identity.Type, entity->Configuration))
			{
				ok = false;
				LogMessage(LOG_ERROR, "Failed to set configuration for entity " + entity->Identity.Identifier, "DiScenFw");
			}
		}
		CyberSystem->Initialize(true);
		for (const auto& agentPair : CyberSystemAgents)
		{
			agentPair.second->ResetAgent();
		}
		return ok;
	}


	bool DigitalScenarioFramework::SaveSystemConfigurationToScenario()
	{
		if (!CheckCyberSystem())
		{
			return false;
		}
		InitDigitalScenario();
		for (const auto& entity : DigitalScenarioManager->GetData()->Entities)
		{
			entity->Configuration = CyberSystem->ReadEntityConfiguration(entity->Identity.Identifier);
		}

		return true;
	}


	const std::shared_ptr<EnvironmentModel> DigitalScenarioFramework::GetSystemModel() const
	{
		if (!CheckCyberSystem())
		{
			return nullptr;
		}
		return CyberSystem->GetModel();
	}


	bool DigitalScenarioFramework::CreateCatalog(const std::string& catalogName)
	{
		if (catalogName.empty())
		{
			return false;
		}
		Catalogs[catalogName] = std::make_shared<Catalog>();
		Catalogs[catalogName]->Name = catalogName;
		return false;
	}


	bool DigitalScenarioFramework::CatalogExists(const std::string& catalogName)
	{
		if (Catalogs.find(catalogName) != Catalogs.end())
		{
			return true;
		}
		return false;
	}


	bool DigitalScenarioFramework::AddCatalogItem(
		const std::string& catalogName,
		const CatalogItem& item
	)
	{
		if (Catalogs.find(catalogName) == Catalogs.end())
		{
			return false;
		}
		Catalogs[catalogName]->Items.push_back(item);
		return true;
	}


	bool DigitalScenarioFramework::SetCatalogCategoryAbbreviation(
		const std::string& catalogName,
		const std::string& category,
		const std::string& abbreviation
	)
	{
		if (!CheckCatalog(catalogName))
		{
			return false;
		}
		if (category.empty())
		{
			return false;
		}
		if (abbreviation.empty())
		{
			// If the abbreviation is an empty string the category abbreviation,
			// if present, is removed.
			if (Catalogs[catalogName]->CategoryAbbreviations.find(category) !=
				Catalogs[catalogName]->CategoryAbbreviations.end())
			{
				Catalogs[catalogName]->CategoryAbbreviations.erase(category);
			}
			return true;
		}

		Catalogs[catalogName]->CategoryAbbreviations[category] = abbreviation;
		return true;
	}


	bool DigitalScenarioFramework::SaveCatalog(const std::string& catalogName, const std::string& jsonPath)
	{
		if (!CheckCatalog(catalogName))
		{
			return false;
		}
		std::string jsonText;
		scen::CatalogToJson(Catalogs[catalogName], jsonText);
		if (!gpvulc::SaveText(jsonPath, jsonText))
		{
			LogMessage(LOG_ERROR, "Failed to save catalog to " + jsonPath, "DiScenFw");
			return false;
		}
		return true;
	}


	const std::shared_ptr<Catalog> DigitalScenarioFramework::LoadCatalog(const std::string& catalogName, const std::string& jsonPath)
	{
		std::shared_ptr<Catalog> catalog = LoadCatalogFromJson(jsonPath);
		if (catalog)
		{
			Catalogs[catalogName] = catalog;
		}
		return catalog;
	}


	const std::shared_ptr<Catalog> DigitalScenarioFramework::GetCatalog(const std::string& catalogName) const
	{
		if (Catalogs.find(catalogName) == Catalogs.end())
		{
			return nullptr;
		}
		return Catalogs.at(catalogName);
	}


	bool DigitalScenarioFramework::LoadScenario(const std::string& jsonPath, bool syncVe)
	{
		InitDigitalScenario();
		bool result = DigitalScenarioManager->LoadScenarioJson(jsonPath, syncVe);
		if (ScenarioSimulationManager)
		{
			ScenarioSimulationManager->SetScenarioData(DigitalScenarioManager->GetData());
		}
		if (result)
		{
			for (const auto& entity : DigitalScenarioManager->GetData()->GetEntities())
			{
				const std::string& catalogName = entity->Asset.Catalog;
				if (!catalogName.empty() && Catalogs.find(catalogName) == Catalogs.end())
				{
					LoadCatalog(catalogName + "Catalog.json", catalogName);
				}
			}
		}
		return result;
	}


	bool DigitalScenarioFramework::SaveScenario(const std::string& jsonPath, bool syncFromVe)
	{
		InitDigitalScenario();
		return DigitalScenarioManager->SaveScenarioJson(jsonPath, syncFromVe);
	}

	void DigitalScenarioFramework::CreateSampleData()
	{
		DigitalScenario()->CreateSampleData();
	}

	std::shared_ptr<Scenario> DigitalScenarioFramework::ScenarioData()
	{
		InitDigitalScenario();
		return DigitalScenarioManager->GetData();
	}


	std::shared_ptr<scen::ScenarioManager> DigitalScenarioFramework::DigitalScenario()
	{
		InitDigitalScenario();
		return DigitalScenarioManager;
	}


	std::shared_ptr<sim::SimulationManager> DigitalScenarioFramework::ScenarioSimulation()
	{
		InitScenarioSimulation();
		return ScenarioSimulationManager;
	}


	std::shared_ptr<sim::SimulationController> DigitalScenarioFramework::ScenarioSimulationController()
	{
		InitScenarioSimulation();
		return ScenarioSimulationManager->GetSimulationController();
	}


	const std::vector<std::shared_ptr<Entity>>& DigitalScenarioFramework::GetEntities()
	{
		InitDigitalScenario();
		return DigitalScenarioManager->GetData()->GetEntities();
	}


	bool DigitalScenarioFramework::LoadScenarioHistory(const std::string& jsonPath)
	{
		InitScenarioSimulation();
		return ScenarioSimulationManager->LoadHistoryJson(jsonPath);
	}


	bool DigitalScenarioFramework::SaveScenarioHistory(const std::string& jsonPath)
	{
		InitScenarioSimulation();
		return ScenarioSimulationManager->SaveHistoryJson(jsonPath);
	}


	bool DigitalScenarioFramework::EntityExists(const std::string& entityId)
	{
		InitDigitalScenario();

		return DigitalScenarioManager->ContainsEntity(entityId);
	}


	const std::shared_ptr<Entity> DigitalScenarioFramework::GetEntity(const std::string& entityId)
	{
		InitDigitalScenario();
		static std::shared_ptr<Entity> entity;
		entity = DigitalScenarioManager->GetEntityById(entityId);
		return entity;
	}


	const std::shared_ptr<Element> DigitalScenarioFramework::GetElement(const std::string& elementId)
	{
		InitDigitalScenario();
		std::shared_ptr<Element> element = DigitalScenarioManager->GetElementById(elementId);
		return element;
	}


	std::shared_ptr<Entity> DigitalScenarioFramework::CreateEntity(
		const std::string& entityClass,
		const EntityIdentity& entityIdentity,
		bool updateXp
		)
	{
		const std::string& entityId = entityIdentity.Identifier;

		if (EntityExists(entityId))
		{
			LogMessage(LOG_ERROR, "The entity " + entityId + " already exists, cannot create it again.", "DiScenFw", true, true, "ENTITY_ALREADY_EXISTS");
			return nullptr;
		}
		//ClearAllExperiences();
		std::shared_ptr< Entity > newEntity = MakeEntity(entityClass);
		if (!newEntity)
		{
			LogMessage(LOG_ERROR, "Invalid entity type '" + entityClass + "', cannot create it.", "DiScenFw", true, true, "NOT_AN_ENTITY");
			return nullptr;
		}
		newEntity->Identity = entityIdentity;
		AddEntity(newEntity, updateXp);
		return newEntity;
	}


	std::shared_ptr< Entity > DigitalScenarioFramework::UpdateEntityData(
		const EntityIdentity& entityIdentity,
		bool updateXp
		)
	{
		InitDigitalScenario();
		std::shared_ptr< Entity > entity = DigitalScenarioManager->GetEntityById(entityIdentity.Identifier);
		if (!entity)
		{
			LogMessage(LOG_ERROR, "Entity '" + entityIdentity.Identifier + "' not found, cannot update it.", "DiScenFw", true, true, "ENTITY_NOT_FOUND");
			return nullptr;
		}
		entity->Identity = entityIdentity;
		return entity;
	}


	std::shared_ptr<Entity> DigitalScenarioFramework::DuplicateEntity(
		const std::string& entityId,
		const std::string& newEntityId,
		bool updateXp
		)
	{
		InitDigitalScenario();
		const std::shared_ptr< Entity > entity = DigitalScenarioManager->GetEntityById(entityId);
		if (!entity)
		{
			LogMessage(LOG_ERROR, "Entity " + entityId + " not found, cannot duplicate it.", "DiScenFw", true, true, "ENTITY_NOT_FOUND");
			return nullptr;
		}
		if (!entityId.empty() && EntityExists(newEntityId))
		{
			LogMessage(LOG_ERROR, "An entity named " + newEntityId + " already exists, cannot duplicate it with the same identifier.", "DiScenFw", true, true, "ENTITY_ALREADY_EXISTS");
			return nullptr;
		}
		//ClearAllExperiences();
		std::shared_ptr< Entity > newEntity = CloneEntity(entity);
		if (!newEntity)
		{
			return nullptr;
		}
		if (newEntityId.empty())
		{
			std::string cleanId = entityId;
			size_t underscorePos = entityId.rfind('_');
			if (
				underscorePos != std::string::npos
				&& entityId.length() > underscorePos
				&& std::isdigit(entityId[underscorePos + 1], std::locale())
				)
			{
				cleanId = entityId.substr(0, underscorePos);
			}
			std::string newId = entityId;
			int count = 0;

			do
			{
				count++;
				std::ostringstream oStream;
				oStream << cleanId << "_" << std::setfill('0') << std::setw(3) << count;
				newId = oStream.str();
			} while (DigitalScenarioManager->ContainsEntity(newId));
			newEntity->Identity.Identifier = newId;
		}
		else
		{
			newEntity->Identity.Identifier = newEntityId;
		}
		AddEntity(newEntity, updateXp);
		return newEntity;
	}


	bool DigitalScenarioFramework::DeleteEntity(const std::string& entityId, bool updateXp)
	{
		if (!CheckCyberSystem())
		{
			return false;
		}
		std::shared_ptr<Entity> entity = DigitalScenario()->GetEntityById(entityId);
		if (!entity)
		{
			return false;
		}

		if (!CyberSystem->RemoveEntity(entityId))
		{
			return false;
		}

		//ClearAllExperiences();
		DigitalScenarioManager->GetData()->RemoveEntity(entity);

		//ConfigureSystemFromScenario();

		const EnvironmentState& systemState = CyberSystem->InterpretSystemState();
		const auto model = CyberSystem->GetModel();
		const std::shared_ptr<EnvironmentState> environment = model->GetStoredState(systemState);
		Transition transition(
			model->GetCurrentState(),
			model->EncodeAction({ "DELETE", {entityId} }),
			environment
			);
		for (auto& agent : CyberSystemAgents)
		{
			// TODO: a set of default actions must be defined
			agent.second->AcquireTransition(
				transition,
				updateXp);
		}

		return true;
	}


	bool DigitalScenarioFramework::CreateAgent(
		const std::string& agentName,
		const std::string& roleName,
		const std::string& arenaName,
		std::function<std::shared_ptr<IAgent>()> makeAgentCallback
		)
	{
		if (!CheckCyberSystem())
		{
			return false;
		}
		if (AgentExists(agentName))
		{
			LogMessage(LOG_ERROR, "The agent \"" + agentName + "\" already exists", "DiScenFw");
			return false;
		}
		std::shared_ptr<SharedArena> arena;
		if (!arenaName.empty())
		{
			auto arenaPair = SharedArenas.find(arenaName);
			if (arenaPair == SharedArenas.end())
			{
				LogMessage(LOG_WARNING, "The agent \"" + agentName + "\" already exists", "DiScenFw");
				return false;
			}

			arena = SharedArenas[arenaName];
		}
		const std::shared_ptr<EnvironmentModel> model = CyberSystem->GetModel();
		if (!model)
		{
			LogMessage(LOG_ERROR, "No model defined for cyber system \"" + CyberSystem->GetSystemName() + "\".", "DiScenFw");
			return false;
		}
		CyberSystem->InitRoles();
		std::string actualRoleName = roleName;
		const auto& roleNames = model->GetRoleNames();
		if (actualRoleName.empty())
		{
			if (!roleNames.empty())
			{
				actualRoleName = roleNames.front(); // get the first role defined
				LogMessage(LOG_DEBUG, "Using role \"" + actualRoleName + "\" from model \"" + model->GetName() + "\".", "DiScenFw");
			}
			else
			{
				LogMessage(LOG_ERROR, "No role defined for model \"" + model->GetName() + "\".", "DiScenFw");
				return false;
			}

		}

		if (std::find(roleNames.cbegin(), roleNames.cend(), actualRoleName) == roleNames.cend())
		{
			LogMessage(LOG_ERROR, "Role \"" + actualRoleName + "\" not defined for model \"" + model->GetName() + "\".", "DiScenFw");
			return false;
		}

		CyberSystemAgents[agentName] = std::make_shared<xp::CyberSystemAgent>(CyberSystem, actualRoleName, agentName);

		if (arena)
		{
			CyberSystemAgents[agentName]->SetArena(arena, true);
		}
		if (makeAgentCallback)
		{
			CyberSystemAgents[agentName]->SetCustomAgentMaker(makeAgentCallback);
		}
		return true;
	}


	bool DigitalScenarioFramework::AgentExists(const std::string& agentName) const
	{
		return (CyberSystemAgents.find(agentName) != CyberSystemAgents.end());
	}


	bool DigitalScenarioFramework::SetCurrentGoal(const std::string& agentName, const std::string& goalName)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		if (CyberSystemAgents.at(agentName)->GetCurrentGoal() == goalName)
		{
			return true;
		}
		ClearCache();

		if (!CyberSystemAgents[agentName]->SetCurrentGoal(goalName))
		{
			return false;
		}
		return ConfigureSystemFromScenario();
	}


	bool DigitalScenarioFramework::RenameCurrentGoal(const std::string& agentName, const std::string& newGoalName)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		ClearCache();

		if (!CyberSystemAgents[agentName]->RenameCurrentGoal(newGoalName))
		{
			return false;
		}
		return ConfigureSystemFromScenario();
	}


	const std::string& DigitalScenarioFramework::GetCurrentGoal(const std::string& agentName) const
	{
		if (!CheckAgent(agentName))
		{
			return EmptyString;
		}
		return CyberSystemAgents.at(agentName)->CurrentExperience()->Goal;
	}


	bool DigitalScenarioFramework::AddNewGoal(const std::string& agentName, const std::string& newGoalName)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		ClearCache();

		if (!CyberSystemAgents[agentName]->AddNewGoal(newGoalName))
		{
			return false;
		}
		return ConfigureSystemFromScenario();
	}


	bool DigitalScenarioFramework::RemoveGoal(const std::string& agentName, const std::string& goalName)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		ClearCache();

		if (!CyberSystemAgents[agentName]->RemoveGoal(goalName))
		{
			return false;
		}
		return ConfigureSystemFromScenario();
	}


	const std::vector<std::string>& DigitalScenarioFramework::GetGoals(const std::string& agentName) const
	{
		if (CheckAgent(agentName))
		{
			return CyberSystemAgents.at(agentName)->GetGoalNames();
		}
		return EmptyStringVector;
	}


	const std::string& DigitalScenarioFramework::GetAgentModel(const std::string& agentName) const
	{
		if (!CheckAgent(agentName))
		{
			return EmptyString;
		}
		return CyberSystemAgents.at(agentName)->CurrentExperience()->GetModelName();
	}


	const std::string& DigitalScenarioFramework::GetAgentRole(const std::string& agentName) const
	{
		if (!CheckAgent(agentName))
		{
			return EmptyString;
		}
		return CyberSystemAgents.at(agentName)->GetRoleName();
	}


	bool DigitalScenarioFramework::SetAgentRole(const std::string& agentName, const std::string& roleName)
	{
		if (roleName.empty())
		{
			return false;
		}
		if (!CheckAgent(agentName))
		{
			return false;
		}
		if (CyberSystemAgents[agentName]->CurrentExperience()->Role == roleName)
		{
			return true;
		}
		std::shared_ptr<EnvironmentModel> model = CyberSystemAgents[agentName]->CurrentExperience()->GetModel();
		if (!model->HasRole(roleName))
		{
			model->StoreRole(RoleInfo(roleName, model->GetName()));
		}
		CyberSystemAgents[agentName]->CurrentExperience()->Role = roleName;
		return true;
	}


	bool DigitalScenarioFramework::SetSuccessCondition(const std::string& agentName, const xp::Condition& successCondition)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		CyberSystemAgents[agentName]->ResetAgentForCurrentGoal();
		CyberSystemAgents[agentName]->ClearCurrentExperience();
		CyberSystemAgents[agentName]->SetSuccessCondition(successCondition);
		return true;
	}


	const xp::Condition& DigitalScenarioFramework::GetSuccessCondition(const std::string& agentName) const
	{
		if (!CheckAgent(agentName))
		{
			return EmptyCondition;
		}
		return CyberSystemAgents.at(agentName)->GetRole()->SuccessCondition;
	}


	bool DigitalScenarioFramework::SetFailureCondition(const std::string& agentName, const xp::Condition& failureCondition)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		CyberSystemAgents[agentName]->ResetAgentForCurrentGoal();
		CyberSystemAgents[agentName]->ClearCurrentExperience();
		CyberSystemAgents[agentName]->SetFailureCondition(failureCondition);
		return true;
	}


	const xp::Condition& DigitalScenarioFramework::GetFailureCondition(const std::string& agentName) const
	{
		if (!CheckAgent(agentName))
		{
			return EmptyCondition;
		}
		const xp::Condition& failureCondition = CyberSystemAgents.at(agentName)->GetRole()->FailureCondition;
		return failureCondition;
	}


	bool DigitalScenarioFramework::SetSystemFailureIgnored(const std::string& agentName, bool ignored)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		CyberSystemAgents[agentName]->ResetAgentForCurrentGoal();
		CyberSystemAgents[agentName]->ClearCurrentExperience();
		CyberSystemAgents.at(agentName)->CurrentExperience()->SystemFailureIgnored = ignored;
		return true;
	}


	bool DigitalScenarioFramework::IsSystemFailureIgnored(const std::string& agentName)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		return CyberSystemAgents.at(agentName)->CurrentExperience()->SystemFailureIgnored;
	}


	bool DigitalScenarioFramework::SetRewardRules(const std::string& agentName, const StateRewardRules& stateReward)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		CyberSystemAgents[agentName]->ResetAgentForCurrentGoal();
		CyberSystemAgents[agentName]->ClearCurrentExperience();
		CyberSystemAgents[agentName]->GetRole()->SetStateReward(stateReward);
		return true;
	}


	const xp::StateRewardRules& DigitalScenarioFramework::GetRewardRules(const std::string& agentName)const
	{
		if (!CheckAgent(agentName))
		{
			return EmptyStateReward;
		}
		return CyberSystemAgents.at(agentName)->GetRole()->StateReward;
	}


	bool DigitalScenarioFramework::SetDiscountingConstant(const std::string& agentName, float discountingConstant)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		CyberSystemAgents.at(agentName)->CurrentExperience()->DiscountingConstant = discountingConstant;
		return false;
	}


	float DigitalScenarioFramework::GetDiscountingConstant(const std::string& agentName) const
	{
		if (!CheckAgent(agentName))
		{
			return -2.0f;
		}
		return CyberSystemAgents.at(agentName)->CurrentExperience()->DiscountingConstant;
	}


	bool DigitalScenarioFramework::CreateSharedArena(const std::string& name, const std::vector<std::string>& assistantNames)
	{
		if (SharedArenas.find(name) != SharedArenas.end())
		{
			LogMessage(LOG_WARNING, "The shared arena \"" + name + "\" already exists.", "DiScenFw");
			return false;
		}
		std::vector< std::shared_ptr<xp::CyberSystemAssistant> > actors;
		for (const std::string& actorName : assistantNames)
		{

			if (CyberSystemAgents.find(actorName) != CyberSystemAgents.end())
			{
				LogMessage(LOG_WARNING, "Agent \"" + actorName + "\" not found.", "DiScenFw");
				return false;
			}
			actors.push_back(CyberSystemAgents[actorName]);
		}
		SharedArenas[name] = std::make_shared<xp::SharedArena>();
		SharedArenas[name]->Actors = actors;
		CurrentSharedArena = SharedArenas[name];
		CurrentSharedArena->Initialize();
		return true;
	}


	bool DigitalScenarioFramework::GetSharedArenaActors(const std::string& name, std::vector<std::string>& assistantNames) const
	{
		if (SharedArenas.find(name) != SharedArenas.cend())
		{
			LogMessage(LOG_WARNING, "The shared arena \"" + name + "\" does not exist.", "DiScenFw");
			return false;
		}
		for (const std::shared_ptr<xp::CyberSystemAssistant>& actor : SharedArenas.at(name)->Actors)
		{
			assistantNames.push_back(actor->GetId());
		}
		return true;
	}


	bool DigitalScenarioFramework::NewEpisode(const std::string& agentName)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		ClearCache();
		CyberSystemAgents[agentName]->NewEpisode();
		return true;
	}


	const std::shared_ptr<xp::EnvironmentState> DigitalScenarioFramework::GetLastScenarioState(const std::string& agentName) const
	{
		if (!CheckAgent(agentName))
		{
			return nullptr;
		}
		const std::shared_ptr<xp::EnvironmentState> envState = CyberSystemAgents.at(agentName)->GetLastState();
		if (!envState || envState->EntityStates.empty())
		{
			return GetLastSystemState();
		}
		return envState;
	}


	const std::shared_ptr<xp::EnvironmentState> DigitalScenarioFramework::GetLastSystemState() const
	{
		if (!CheckCyberSystem())
		{
			return nullptr;
		}
		const EnvironmentState& systemState = CyberSystem->InterpretSystemState();
		const std::shared_ptr<EnvironmentModel> model = CyberSystem->GetModel();
		const std::shared_ptr<EnvironmentState> state = model->GetStoredState(systemState);
		return state;
	}


	const std::string& DigitalScenarioFramework::GetSystemInfo(const std::string& infoId) const
	{
		if (!CheckCyberSystem())
		{
			LastSystemInfo.clear();
		}
		else
		{
			LastSystemInfo = CyberSystem->GetSystemInfo(infoId);
		}

		return LastSystemInfo;
	}


	const xp::ActionOutcome DigitalScenarioFramework::GetLastActionOutcome(const std::string& agentName) const
	{
		if (!CheckAgent(agentName))
		{
			return xp::ActionOutcome(ActionError::AGENT_NOT_FOUND);
		}
		if (CachedLastActionOutcome.find(agentName) == CachedLastActionOutcome.cend())
		{
			return xp::ActionOutcome(ActionError::NO_ACTION_FOUND);
		}
		return CachedLastActionOutcome.at(agentName);
	}


	const std::vector<Action>& DigitalScenarioFramework::GetSuggestedActions(const std::string& agentName) const
	{
		if (CheckAgent(agentName))
		{
			return CyberSystemAgents.at(agentName)->GetSuggestedActions();
		}
		return EmptyActionVector;
	}


	const std::vector<Action>& DigitalScenarioFramework::GetForbiddenActions(const std::string& agentName) const
	{
		if (CheckAgent(agentName))
		{
			return CyberSystemAgents.at(agentName)->GetForbiddenActions();
		}
		return EmptyActionVector;
	}


	const std::vector<ActionRef>& DigitalScenarioFramework::GetAvailableActions(
		const std::string& roleId,
		bool smartSelection) const
	{
		if (CyberSystem)
		{
			return CyberSystem->GetAvailableActions(roleId, smartSelection);
		}
		return EmptyActionRefVector;
	}


	const bool DigitalScenarioFramework::EpisodeStarted(const std::string& agentName) const
	{
		if (CheckAgent(agentName))
		{
			return CyberSystemAgents.at(agentName)->EpisodeStarted();
		}
		return false;
	}


	const std::vector<Action>& DigitalScenarioFramework::GetActionsSequence(const std::string& agentName) const
	{
		if (CheckAgent(agentName))
		{
			return CyberSystemAgents.at(agentName)->GetLastActionsSequence();
		}
		return EmptyActionVector;
	}


	ActionOutcome DigitalScenarioFramework::Train(const std::string& agentName, bool updateXp, xp::AgentMode rlMode)
	{
		bool learn = (rlMode & xp::AgentMode::LEARN) == xp::AgentMode::LEARN;
		bool enableLoopDetection = (rlMode & xp::AgentMode::LOOP_DETECTION) == xp::AgentMode::LOOP_DETECTION;
		std::shared_ptr<CyberSystemAgent> agent = GetAgent(agentName);
		if (!agent)
		{
			return xp::ActionOutcome(ActionError::AGENT_NOT_FOUND);
		}
		agent->SetLoopDetectionEnabled(enableLoopDetection);
		ActionResult prevResult = agent->LastEpisodeResult();
		agent->SetLearningEnabled(learn);
		ActionResult result = agent->Train(updateXp);

		xp::ActionOutcome outcome = ProcessEpisode(
			agent,
			prevResult,
			result
			);

		return outcome;
	}


	xp::ActionOutcome DigitalScenarioFramework::TakeAction(const std::string& agentName, const xp::Action& action, bool updateXp, xp::AgentMode rlMode)
	{
		std::shared_ptr<CyberSystemAgent> agent = GetAgent(agentName);
		if (!agent)
		{
			return xp::ActionOutcome(ActionError::AGENT_NOT_FOUND);
		}
		bool learn = (rlMode & xp::AgentMode::LEARN) == xp::AgentMode::LEARN;
		bool enableLoopDetection = (rlMode & xp::AgentMode::LOOP_DETECTION) == xp::AgentMode::LOOP_DETECTION;
		agent->SetLoopDetectionEnabled(enableLoopDetection);
		agent->SetLearningEnabled(learn);
		ActionResult prevResult = agent->LastEpisodeResult();
		ActionResult result = agent->TakeAction(action, updateXp);
		xp::ActionOutcome outcome = ProcessEpisode(
			agent,
			prevResult,
			result
			);

		return outcome;
	}


	const std::shared_ptr<xp::IAgentConfiguration> DigitalScenarioFramework::GetAgentConfiguration(const std::string& agentName)
	{
		std::shared_ptr<CyberSystemAgent> agent = GetAgent(agentName);
		if (!agent)
		{
			return nullptr;
		}
		return agent->GetAgentConfiguration();
	}


	bool DigitalScenarioFramework::SetAgentConfiguration(
		const std::string& agentName,
		const std::shared_ptr<xp::IAgentConfiguration> config
		)
	{
		std::shared_ptr<CyberSystemAgent> agent = GetAgent(agentName);
		if (!agent)
		{
			return false;
		}
		agent->SetAgentConfiguration(config);
		return true;
	}


	bool DigitalScenarioFramework::LoadRLConfiguration(const std::string& agentName, const std::string& filePath)
	{
		std::shared_ptr<CyberSystemAgent> agent = GetAgent(agentName);
		if (!agent)
		{
			return false;
		}
		if (filePath.empty())
		{
			return false;
		}
		std::string jsonText;
		if (!gpvulc::LoadText(filePath, jsonText))
		{
			LogMessage(LOG_ERROR, "Failed to load RL configuration from " + filePath, "DiScenFw");
			return false;
		}
		std::shared_ptr<RLConfig> rlConfig = std::make_shared<RLConfig>();
		std::string errMsg;
		bool failed = false;
		try
		{
			xp::RLConfigFromJson(jsonText, *rlConfig);
		}
		catch (gpvulc::json::ParseException parseException)
		{
			errMsg = GetParseExceptionErrorMessage(parseException);
			failed = true;
		}
		catch (gpvulc::json::FormatException formatException)
		{
			errMsg = std::string("JSON assert failed: ") + formatException.what();
			failed = true;
		}
		catch (gpvulc::json::ContentException contentException)
		{
			errMsg = contentException.what();
			failed = true;
		}

		if (failed)
		{
			if (!errMsg.empty())
			{
				errMsg = "Error reading JSON from " + filePath + ": " + errMsg;
				LogMessage(LOG_ERROR, errMsg, "DiScenFw");
			}
			return false;
		}
		agent->SetAgentConfiguration(rlConfig);
		return true;
	}


	bool DigitalScenarioFramework::SaveRLConfiguration(const std::string& agentName, const std::string& filePath)
	{
		std::shared_ptr<CyberSystemAgent> agent = GetAgent(agentName);
		if (!agent)
		{
			return false;
		}
		std::shared_ptr<IAgentConfiguration> config = agent->GetAgentConfiguration();
		if (!config || !config->IsA("RLConfig"))
		{
			LogMessage(LOG_WARNING, "Invalid agent configuration: cannot serialize.", "DiScenFw");
			return false;
		}
		std::shared_ptr<RLConfig> rlConfig = std::static_pointer_cast<RLConfig>(config);
		std::string jsonText;
		xp::RLConfigToJson(*rlConfig, jsonText);
		if (!gpvulc::SaveText(filePath, jsonText))
		{
			LogMessage(LOG_ERROR, "Failed to save RL configuration to " + filePath, "DiScenFw");
			return false;
		}
		return true;
	}


	xp::AgentStats DigitalScenarioFramework::GetAgentStats(const std::string& agentName)
	{
		std::shared_ptr<CyberSystemAgent> agent = GetAgent(agentName);
		if (!agent)
		{
			return xp::AgentStats();
		}
		return agent->GetStatistics();
	}


	bool DigitalScenarioFramework::ResetAgentStats(const std::string& agentName)
	{
		std::shared_ptr<CyberSystemAgent> agent = GetAgent(agentName);
		if (!agent)
		{
			return false;
		}
		agent->ResetStats();
		return true;
	}


	bool DigitalScenarioFramework::ResetAgent(const std::string& agentName)
	{
		std::shared_ptr<CyberSystemAgent> agent = GetAgent(agentName);
		if (!agent)
		{
			return false;
		}
		agent->ResetAgent();
		return true;
	}


	bool DigitalScenarioFramework::SetLevel(const std::string& agentName, xp::ExperienceLevel level)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		CyberSystemAgents[agentName]->SetLevel(level);
		return true;
	}


	ExperienceLevel DigitalScenarioFramework::GetLevel(const std::string& agentName)
	{
		if (!CheckAgent(agentName))
		{
			return ExperienceLevel::UNKNOWN;
		}
		return CyberSystemAgents[agentName]->GetLevel();
	}


	bool DigitalScenarioFramework::LoadExperience(const std::string& agentName, const std::string& filePath)
	{
		ClearCache();
		if (!CheckAgent(agentName))
		{
			return false;
		}

		CyberSystem->Clear();
		bool result = CyberSystemAgents[agentName]->LoadExperience(filePath, true);

		//CyberSystem->Initialize(true);
		//ResetCyberSystem();

		result = ConfigureSystemFromScenario() && result;
		return result;
	}


	bool DigitalScenarioFramework::SaveExperience(const std::string& agentName, const std::string& filePath, const std::string& goalName)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}

		return CyberSystemAgents[agentName]->SaveExperience(filePath, goalName, true);
	}


	bool DigitalScenarioFramework::OptimizeExperienceForAssistance(const std::string& agentName)
	{
		ClearCache();
		if (!CheckAgent(agentName))
		{
			return false;
		}
		return CyberSystemAgents[agentName]->OptimizeForAssistance();
	}


	bool DigitalScenarioFramework::ClearExperience(const std::string& agentName)
	{
		ClearCache();
		if (!CheckAgent(agentName))
		{
			return false;
		}
		CyberSystemAgents[agentName]->ResetAgentForCurrentGoal();
		CyberSystemAgents[agentName]->ClearCurrentExperience();
		return true;
	}


	void DigitalScenarioFramework::ClearAllExperiences()
	{
		ClearCache();
		for (const auto& agentPair : CyberSystemAgents)
		{
			agentPair.second->ResetAgent();
			agentPair.second->ClearAllExperiences();
		}
	}


	bool DigitalScenarioFramework::SetLogEnabled(const std::string& agentName, bool enabled)
	{
		if (!CheckAgent(agentName))
		{
			return false;
		}
		CyberSystemAgents[agentName]->SetLogEnabled(enabled);
		return true;
	}



	void DigitalScenarioFramework::ResetScenario()
	{
		ClearCache();
		if (DigitalScenarioManager)
		{
			DigitalScenarioManager->ResetScenario();
		}
		if (ScenarioSimulationManager)
		{
			ScenarioSimulationManager->ClearSimulation();
			ScenarioSimulationManager->SetScenarioData(DigitalScenarioManager->GetData());
		}
	}


	void DigitalScenarioFramework::ResetXp()
	{
		SharedArenas.clear();
		CyberSystemAgents.clear();
		CurrentSharedArena.reset();
		ClearCache();
	}


	void DigitalScenarioFramework::ResetCyberSystem()
	{
		if (CyberSystem)
		{
			CyberSystem->ResetSystem();
			CyberSystem->SetConfiguration("");
			CyberSystem->Initialize(true);
		}
	}


	void DigitalScenarioFramework::ResetAll()
	{
		ResetScenario();
		ResetXp();
		EnvironmentModel::RemoveAllModels();
		//ResetCyberSystem();
		if (CyberSystem)
		{
			CyberSystem->Clear();
		}
	}


	void DigitalScenarioFramework::InitCyberSystem()
	{
		if (!CyberSystem)
		{
			CyberSystem = std::make_shared<xp::CyberSystemLink>();
		}
	}


	bool DigitalScenarioFramework::CheckCyberSystem() const
	{
		if (!CyberSystem || !CyberSystem->IsCyberSystemLoaded())
		{
			LogMessage(LOG_ERROR, "No cyber system available", "DiScenFw");
			return false;
		}
		return true;
	}


	void DigitalScenarioFramework::InitDigitalScenario()
	{
		if (!DigitalScenarioManager)
		{
			DigitalScenarioManager = std::make_shared<scen::ScenarioManager>();
		}
	}


	void DigitalScenarioFramework::InitScenarioSimulation()
	{
		if (!ScenarioSimulationManager)
		{
			InitDigitalScenario();
			ScenarioSimulationManager = std::make_shared<sim::SimulationManager>();
			ScenarioSimulationManager->SetScenarioData(DigitalScenarioManager->GetData());
		}
	}


	std::shared_ptr<Catalog> DigitalScenarioFramework::ParseCatalogJson(const std::string& jsonText)
	{
		std::shared_ptr<Catalog> catalog;
		std::string errMsg;
		try
		{
			catalog = scen::CatalogFromJson(jsonText);
		}
		catch (gpvulc::json::ParseException parseException)
		{
			errMsg = GetParseExceptionErrorMessage(parseException);
		}
		catch (gpvulc::json::FormatException formatException)
		{
			errMsg = std::string("JSON assert failed: ") + formatException.what();
		}
		catch (gpvulc::json::ContentException contentException)
		{
			errMsg = contentException.what();
		}

		if (!catalog)
		{
			if (!errMsg.empty())
			{
				LogMessage(LOG_ERROR, "Error reading JSON:\n" + errMsg, "DiScenFw");
			}
			return nullptr;
		}
		return catalog;
	}


	std::shared_ptr<Catalog> DigitalScenarioFramework::LoadCatalogFromJson(const std::string& jsonPath)
	{
		if (jsonPath.empty())
		{
			return nullptr;
		}
		std::string jsonText;
		if (!gpvulc::LoadText(jsonPath, jsonText))
		{
			LogMessage(LOG_ERROR, "Failed to load catalog from " + jsonPath, "DiScenFw");
			//std::cerr << "Can’t open intput file " << fileName << std::endl;
			return nullptr;
		}
		return ParseCatalogJson(jsonText);
	}


	bool DigitalScenarioFramework::CheckCatalog(const std::string& catalogName)
	{
		if (!CatalogExists(catalogName))
		{
			LogMessage(LOG_ERROR, "Catalog '" + catalogName + "' not found.", "DiScenFw", true, true, "CHECK_CATALOG");
			return false;
		}
		return true;
	}


	std::shared_ptr<xp::CyberSystemAgent> DigitalScenarioFramework::GetAgent(const std::string& agentName)
	{
		if (!CheckAgent(agentName))
		{
			return nullptr;
		}
		return CyberSystemAgents[agentName];
	}


	bool DigitalScenarioFramework::CheckAgent(const std::string& agentName) const
	{
		if (!AgentExists(agentName))
		{
			LogMessage(LOG_ERROR, "Agent '" + agentName + "' not found.", "DiScenFw", true, true, "CHECK_AGENT");
			return false;
		}
		return true;
	}


	void DigitalScenarioFramework::AddEntity(std::shared_ptr<Entity> newEntity, bool updateXp)
	{
		DigitalScenarioManager->GetData()->AddEntity(newEntity);
		if (CheckCyberSystem())
		{
			CyberSystem->ConfigureEntity(newEntity->Identity.Identifier, newEntity->Identity.Type, newEntity->Configuration);
			const EnvironmentState& systemState = CyberSystem->InterpretSystemState();
			const auto model = CyberSystem->GetModel();
			const std::shared_ptr<EnvironmentState> environment = model->GetStoredState(systemState);
			Transition transition(
				model->GetCurrentState(),
				model->EncodeAction({ "ADD", {newEntity->Identity.Identifier} }),
				environment
				);
			for (auto& agentPair : CyberSystemAgents)
			{
				// TODO: a set of default actions must be defined
				agentPair.second->AcquireTransition(
					transition,
					updateXp);
			}
		}
	}


	void DigitalScenarioFramework::ClearCache()
	{
		CachedLastActionOutcome.clear();
	}


	xp::ActionOutcome DigitalScenarioFramework::ProcessEpisode(
		std::shared_ptr<xp::CyberSystemAgent> agent,
		ActionResult prevResult,
		ActionResult result
		)
	{
		bool inArena = CurrentSharedArena &&
			std::find(
				CurrentSharedArena->Actors.begin(),
				CurrentSharedArena->Actors.end(),
				agent)
			!= CurrentSharedArena->Actors.end();

		bool multiAgent = inArena && CurrentSharedArena->MultiActor();

		bool completedEpisode = false;
		if (result != ActionResult::IN_PROGRESS && prevResult != result)
		{
			completedEpisode = true;
		}

		// Stuck field (invasive changes also in .NET binding)

		xp::ActionOutcome outcome
		{
			result,
			multiAgent,
			agent->IsNewEpisode(),
			completedEpisode,
			result == ActionResult::DENIED,
			agent->GetLastStateReward(),
			agent->GetLastEpisodePerformance()
		};
		CachedLastActionOutcome[agent->GetId()] = outcome;

		return outcome;
	}
} // namespace discenfw


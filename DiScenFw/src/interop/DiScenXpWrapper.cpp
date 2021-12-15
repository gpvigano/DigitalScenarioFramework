//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/CyberSystemAgent.h>
#include <discenfw/xp/Condition.h>
#include <discenfw/scen/ScenarioData.h>
#include <discenfw/xp/DiScenXpWrapper.h>
#include <discenfw/interop/CyberSystemLink.h>
#include <discenfw/xp/EnvironmentModel.h>
#include <discenfw/DigitalScenarioFramework.h>

#include <DiScenXp.h>

#include <discenfw/util/MessageLog.h>

namespace discenfw
{
	namespace xp
	{

		std::shared_ptr<DiScenXpWrapper> DiScenXpWrapper::Instance;


		std::shared_ptr<DiScenXpWrapper> DiScenXpWrapper::GetInstance()
		{
			if (!Instance)
			{
				Instance = std::shared_ptr<DiScenXpWrapper>(new DiScenXpWrapper());
			}

			return Instance;
		}


		void DiScenXpWrapper::DestroyInstance()
		{
			if (Instance)
			{
				Instance = nullptr;
			}
		}


		void DiScenXpWrapper::UpdateActionsCache()
		{
			if (AvailableActionsCache.empty())
			{
				AvailableActionsCache = DiScenFw()->GetAvailableActions(GetAgentName());
				AvailableActionsData.resize(AvailableActionsCache.size());
				for (size_t i = 0; i < AvailableActionsCache.size(); i++)
				{
					ExportAction(*(AvailableActionsCache[i]), AvailableActionsData[i]);
				}
			}
			if (ForbiddenActionsCache.empty())
			{
				ForbiddenActionsCache = DiScenFw()->GetForbiddenActions(GetAgentName());
				ForbiddenActionsData.resize(ForbiddenActionsCache.size());
				for (size_t i = 0; i < ForbiddenActionsCache.size(); i++)
				{
					ExportAction(ForbiddenActionsCache[i], ForbiddenActionsData[i]);
				}
			}
			if (SuggestedActionsCache.empty())
			{
				SuggestedActionsCache = DiScenFw()->GetSuggestedActions(GetAgentName());
				SuggestedActionsData.resize(SuggestedActionsCache.size());
				for (size_t i = 0; i < SuggestedActionsCache.size(); i++)
				{
					ExportAction(SuggestedActionsCache[i], SuggestedActionsData[i]);
				}
			}
			//LogMessage(DEBUG, "AvailableActions=" + std::to_string(AvailableActionsCache.size())
			//	+ " ForbiddenActions=" + std::to_string(ForbiddenActionsCache.size())
			//	+ " SuggestedActions=" + std::to_string(SuggestedActionsCache.size()), "DiScenFw(xp)");
		}


		void DiScenXpWrapper::SetCyberSystemDllDirectory(const char* dllDirPath)
		{
			CyberSystemDllDirectory = dllDirPath;
		}


		Bool DiScenXpWrapper::LoadCyberSystem(const char* dllName)
		{
			if (dllName == nullptr)
			{
				LogMessage(LOG_ERROR, "FATAL ERROR! Failed to load cyber system (null).", "DiScenFw(xp)");
				return Bool::FALSE;
			}
			if (CyberSystemDllName == dllName)
			{
				LogMessage(LOG, "Cyber system " + CyberSystemDllName + " already loaded.", "DiScenFw(xp)");
				return Bool::TRUE;
			}
			if (!DiScenFw()->LoadCyberSystem(dllName))
			{
				LogMessage(LOG_ERROR, "FATAL ERROR! Failed to load cyber system " + std::string(dllName), "DiScenFw(xp)", true, true);
				return Bool::FALSE;
			}
			CyberSystemDllName = std::string(dllName);
			return Bool::TRUE;
		}


		void DiScenXpWrapper::NewEpisode()
		{
			ClearActionsCache();
			LastActionResult = ActionResult::IN_PROGRESS;
			DiScenFw()->NewEpisode(GetAgentName());
			DiScenFw()->SetLevel(GetAgentName(), ExperienceLevel::ASSISTANT);
		}


		void DiScenXpWrapper::ClearAllExperiences()
		{
			LastActionResult = ActionResult::IN_PROGRESS;
			DiScenFw()->ClearAllExperiences();
			ClearActionsCache();
		}


		void DiScenXpWrapper::ClearCurrentExperience()
		{
			LastActionResult = ActionResult::IN_PROGRESS;
			DiScenFw()->ClearExperience(GetAgentName());
			DiScenFw()->ResetAgent(GetAgentName());
			ClearActionsCache();
		}


		Result DiScenXpWrapper::TakeAction(const char* actionId, const char** params, int paramsCount, Bool updateXp)
		{
			std::vector<std::string> paramStr;
			for (int i = 0; i < paramsCount; i++)
			{
				paramStr.push_back(std::string(params[i]));
			}
			ClearActionsCache();
			LastActionResult = DiScenFw()->TakeAction(GetAgentName(), { actionId,paramStr }, updateXp == TRUE, DeadlockDetectionFlag ? AgentMode::DEFAULT : AgentMode::LEARN).Result;

			return (Result)LastActionResult;
		}


		std::shared_ptr<EnvironmentState> DiScenXpWrapper::GetLastState()
		{
			std::shared_ptr<EnvironmentState> state = DiScenFw()->GetLastScenarioState(GetAgentName());
			if (!state)
			{
				DiScenFw()->NewEpisode(GetAgentName());
				state = DiScenFw()->GetLastScenarioState(GetAgentName());
			}
			return state;
		}


		const char* DiScenXpWrapper::GetEntityProperty(const char* entityId, const char* propertyId)
		{
			std::shared_ptr<EnvironmentState> state = DiScenFw()->GetLastScenarioState(GetAgentName());
			if (!state)
			{
				DiScenFw()->NewEpisode(GetAgentName());
				state = DiScenFw()->GetLastScenarioState(GetAgentName());
			}
			std::shared_ptr<EntityState> entState = state->EntityStates[entityId];
			return entState->PropertyValues[propertyId].c_str();
		}


		Bool DiScenXpWrapper::CheckEntityProperty(const char* entityId,
			const char* propertyId, const char* propertyValue)
		{
			std::shared_ptr<EnvironmentState> state = DiScenFw()->GetLastScenarioState(GetAgentName());
			if (!state)
			{
				DiScenFw()->NewEpisode(GetAgentName());
				state = DiScenFw()->GetLastScenarioState(GetAgentName());
			}
			std::shared_ptr<EntityState> entState = state->EntityStates[entityId];
			if (!state)
			{
				return FALSE;
			}
			bool result = (entState->PropertyValues[propertyId] == std::string(propertyValue));
			return (Bool)result;
		}


		Bool DiScenXpWrapper::EntityHasRelationship(const char* entityId, const char* RelationshipId)
		{
			std::shared_ptr<EnvironmentState> state = GetLastState();
			std::shared_ptr<EntityState> entityState = state->EntityStates[entityId];
			bool found = (entityState->Relationships.find(RelationshipId) != entityState->Relationships.end());
			return (Bool)found;
		}


		int DiScenXpWrapper::GetEntityPropertiesCount(const char* entityId)
		{
			std::shared_ptr<EnvironmentState> state = GetLastState();
			std::shared_ptr<EntityState> entityState = state->EntityStates[entityId];
			int n = (int)entityState->PropertyValues.size();
			return n;
		}


		int DiScenXpWrapper::GetEntityProperties(const char* entityId, PropertyData* properties, int* size)
		{
			std::shared_ptr<EnvironmentState> state = GetLastState();
			std::shared_ptr<EntityState> entityState = state->EntityStates[entityId];
			int n = (int)entityState->PropertyValues.size();
			n = *size > n ? n : *size;
			*size = n;
			int count = 0;
			for (const auto& rel : entityState->PropertyValues)
			{
				properties[count].PropertyId = rel.first.c_str();
				properties[count].PropertyValue = rel.second.c_str();
				count++;
			}
			return count;
		}


		int DiScenXpWrapper::GetScenarioEntitiesCount()
		{
			const std::shared_ptr<EnvironmentState> state = DiScenFw()->GetLastScenarioState(GetAgentName());
			if (!state)
			{
				return 0;
			}
			int count = (int)state->EntityStates.size();
			return count;
		}


		int DiScenXpWrapper::GetScenarioEntities(const char** entityId, int* size)
		{
			const std::shared_ptr<EnvironmentState> state = DiScenFw()->GetLastScenarioState(GetAgentName());
			int count = 0;
			if (state)
			{
				for (const auto& ent : state->EntityStates)
				{
					entityId[count] = ent.first.c_str();
					if (count >= *size) break;
					count++;
				}
			}
			*size = count;
			return count;
		}


		int DiScenXpWrapper::GetChangedEntitiesCount()
		{
			const std::shared_ptr<EnvironmentState> state = LastStateChanges();
			int count = state ? (int)state->EntityStates.size() : 0;
			return count;
		}


		int DiScenXpWrapper::GetChangedEntities(const char** entityId, int* size)
		{
			const std::shared_ptr<EnvironmentState> state = LastStateChanges();
			if (!state)
			{
				return 0;
			}
			int count = 0;
			for (const auto& ent : state->EntityStates)
			{
				if (count >= *size) break;
				entityId[count] = ent.first.c_str();
				count++;
			}
			*size = count;
			return (int)state->EntityStates.size();
		}


		int DiScenXpWrapper::GetEntityRelationshipsCount(const char* entityId)
		{
			std::shared_ptr<EnvironmentState> state = GetLastState();
			std::shared_ptr<EntityState> entityState = state->EntityStates[entityId];
			int n = (int)entityState->Relationships.size();
			return n;
		}


		int DiScenXpWrapper::GetEntityRelationships(const char* entityId, RelationshipData* relationships, int* size)
		{
			std::shared_ptr<EnvironmentState> state = GetLastState();
			std::shared_ptr<EntityState> entityState = state->EntityStates[entityId];
			int n = (int)entityState->Relationships.size();
			n = *size > n ? n : *size;
			*size = n;
			int count = 0;
			for (const auto& rel : entityState->Relationships)
			{
				relationships[count].RelationshipId = rel.first.c_str();
				relationships[count].RelatedEntityId = rel.second.EntityId.c_str();
				relationships[count].RelatedEndPoint = rel.second.LinkId.c_str();
				count++;
			}
			return count;
		}


		const char* DiScenXpWrapper::GetEntityStateType(const char* entityId)
		{
			std::shared_ptr<EnvironmentState> state = GetLastState();
			if (state->EntityStates.find(entityId) == state->EntityStates.end())
			{
				LogMessage(LOG_ERROR, "Error: entity state " + std::string(entityId) + " not found.\n", "DiScenFw(xp)");
				return nullptr;
			}
			std::shared_ptr<EntityState> entityState = state->EntityStates[entityId];
			std::shared_ptr<EntityStateType> entityStateType = entityState->GetType();
			if (entityStateType)
			{
				EntityStateTypeNameBuffer = entityStateType->GetTypeName();
				return EntityStateTypeNameBuffer.c_str();
			}
			LogMessage(LOG_ERROR, "Error: entity state type for " + std::string(entityId) + " not found.\n", "DiScenFw(xp)");
			return nullptr;
		}


		int DiScenXpWrapper::GetPossiblePropertiesCount(const char* entityTypeId)
		{
			std::shared_ptr<EntityStateType> entityStateType = GetLastModel()->GetEntityStateType(entityTypeId);
			if (entityStateType)
			{
				return entityStateType->CountPossibleProperties();
			}
			return 0;
		}


		Bool DiScenXpWrapper::GetPossibleProperties(const char* entityTypeId, const char** propertyIds, int * size)
		{
			std::shared_ptr<EntityStateType> entityStateType = GetLastModel()->GetEntityStateType(entityTypeId);
			if (entityStateType)
			{
				entityStateType->GetPossibleProperties(PropertyIdsCache);
				int count = (int)PropertyIdsCache.size();
				if (*size > count) *size = count;
				count = *size;
				for (int i = 0; i < count; i++)
				{
					propertyIds[i] = PropertyIdsCache[i].c_str();
				}
				return TRUE;
			}
			return FALSE;
		}


		int DiScenXpWrapper::GetPossiblePropertyValuesCount(const char* entityTypeId, const char* propertyId)
		{
			std::shared_ptr<EntityStateType> entityStateType = GetLastModel()->GetEntityStateType(entityTypeId);
			if (entityStateType)
			{
				return entityStateType->CountPossiblePropertyValues(propertyId);
			}
			return 0;
		}


		Bool DiScenXpWrapper::GetPossiblePropertyValues(const char* entityTypeId, const char* propertyId, const char** values, int* size)
		{
			std::shared_ptr<EntityStateType> entityStateType = GetLastModel()->GetEntityStateType(entityTypeId);
			if (entityStateType)
			{
				entityStateType->GetPossiblePropertyValues(propertyId, PropertyValuesCache);
				int count = (int)PropertyValuesCache.size();
				if (*size > count) *size = count;
				count = *size;
				for (int i = 0; i < count; i++)
				{
					values[i] = PropertyValuesCache[i].c_str();
				}
				return TRUE;
			}
			*size = 0;
			return FALSE;
		}


		int DiScenXpWrapper::GetActionParamsCount(ActionData* actionPtr)
		{
			return actionPtr->ParamsCount;
		}


		void DiScenXpWrapper::GetAction(ActionData* actionPtr, const char** actionId, const char** paramsArray, int* paramsCount)
		{
			*actionId = actionPtr->ActionId;
			int pCount = actionPtr->ParamsCount;
			if (pCount < *paramsCount) *paramsCount = pCount;
			for (int i = 0; i < *paramsCount; i++)
			{
				paramsArray[i] = actionPtr->Params[i];
			}
		}


		int DiScenXpWrapper::GetAvailableActionsCount()
		{
			UpdateActionsCache();
			int actionsCount = (int)(AvailableActionsCache.size());
			return actionsCount;
		}



		int DiScenXpWrapper::GetAvailableActions(ActionData** availableActionsData, int* size)
		{
			int actionsCount = GetAvailableActionsCount();
			if (availableActionsData != nullptr && size != nullptr)
			{
				int n = actionsCount > *size ? *size : actionsCount;
				for (int i = 0; i < n; i++)
				{
					availableActionsData[i] = &(AvailableActionsData[i]);
				}
				*size = n;
			}
			return actionsCount;
		}


		int DiScenXpWrapper::GetForbiddenActionsCount()
		{
			UpdateActionsCache();
			int actionsCount = (int)(ForbiddenActionsCache.size());
			return actionsCount;
		}


		int DiScenXpWrapper::GetForbiddenActions(ActionData** forbiddenActionsData, int* size)
		{
			int actionsCount = GetForbiddenActionsCount();
			if (forbiddenActionsData != nullptr && size != nullptr)
			{
				int n = actionsCount > *size ? *size : actionsCount;
				for (int i = 0; i < n; i++)
				{
					forbiddenActionsData[i] = &(ForbiddenActionsData[i]);
				}
				*size = n;
			}
			return actionsCount;
		}


		int DiScenXpWrapper::GetSuggestedActionsCount()
		{
			UpdateActionsCache();
			int actionsCount = (int)(SuggestedActionsCache.size());
			return actionsCount;
		}


		int DiScenXpWrapper::GetSuggestedActions(ActionData** suggestedActionsData, int* size)
		{
			int actionsCount = GetSuggestedActionsCount();
			if (suggestedActionsData != nullptr && size != nullptr)
			{
				int n = actionsCount > *size ? *size : actionsCount;
				for (int i = 0; i < n; i++)
				{
					suggestedActionsData[i] = &(SuggestedActionsData[i]);
				}
				*size = n;
			}
			return actionsCount;
		}


		void DiScenXpWrapper::AddSuccessCondition(const char* entityId, const char* propertyId, const char* propertyValue)
		{
			Condition successCondition({ EntityCondition{entityId, {{ propertyId, propertyValue }}} });
			Condition newSuccessCondition = DiScenFw()->GetSuccessCondition(GetAgentName());
			if (!newSuccessCondition.Defined())
			{
				newSuccessCondition = successCondition;
			}
			else
			{
				newSuccessCondition.AddCondition(LogicOp::AND, successCondition);
			}
			DiScenFw()->SetSuccessCondition(GetAgentName(), newSuccessCondition);
			SuccessConditionsData.clear();
		}


		void DiScenXpWrapper::ResetSuccessCondition()
		{
			DiScenFw()->SetSuccessCondition(GetAgentName(), Condition());
			SuccessConditionsData.clear();
		}


		int DiScenXpWrapper::GetSuccessConditionsCount()
		{
			const std::shared_ptr<RoleInfo> role = xp::GetRole(DiScenFw()->GetAgentRole(GetAgentName()),DiScenFw()->GetAgentModel(GetAgentName()));
			int count = (int)role->GetSuccessCondition().RelatedConditions.size();
			if (!role->GetSuccessCondition().EntityConditions.empty())
			{
				count++;
			}
			return count;
		}


		Bool DiScenXpWrapper::GetSuccessConditions(ConditionData* successConditionsData, int * size)
		{
			int count = GetSuccessConditionsCount();
			if (successConditionsData == nullptr || size == nullptr || *size < count)
			{
				return Bool::FALSE;
			}
			*size = count;
			const Condition& successCond = DiScenFw()->GetSuccessCondition(GetAgentName());
			bool rebuildCache = SuccessConditionsData.size() != count;
			if (rebuildCache)
			{
				SuccessConditionsData.clear();
				SuccessConditionsData.resize(count);

				//TODO: to be updated according to EntityCondition definition

				int i = 0;
				if (!successCond.EntityConditions.empty() && !successCond.EntityConditions.begin()->PropConditions.empty())
				{
					SuccessConditionsData[0].EntityId = successCond.EntityConditions.begin()->EntityId;
					SuccessConditionsData[0].PropertyId = successCond.EntityConditions.begin()->PropConditions[0].PropertyName;
					SuccessConditionsData[0].PropertyValue = successCond.EntityConditions.begin()->PropConditions[0].PropertyValue;
					i++;
				}
				if (!successCond.RelatedConditions.empty())
				{
					for (const auto& relCond : successCond.RelatedConditions)
					{
						if (!relCond.second->EntityConditions.begin()->PropConditions.empty())
						{
							SuccessConditionsData[i].EntityId = relCond.second->EntityConditions.begin()->EntityId;
							SuccessConditionsData[i].PropertyId = relCond.second->EntityConditions.begin()->PropConditions[0].PropertyName;
							SuccessConditionsData[i].PropertyValue = relCond.second->EntityConditions.begin()->PropConditions[0].PropertyValue;
							i++;
						}
					}
				}
			}
			for (int i = 0; i < count; i++)
			{
				const auto& cond = SuccessConditionsData[i];
				successConditionsData[i].EntityId = SuccessConditionsData[i].EntityId.c_str();
				successConditionsData[i].PropertyId = SuccessConditionsData[i].PropertyId.c_str();
				successConditionsData[i].PropertyValue = SuccessConditionsData[i].PropertyValue.c_str();
			}

			return Bool::TRUE;
		}


		void DiScenXpWrapper::SetDeadlockDetection(Bool enabled)
		{
			DeadlockDetectionFlag = (enabled == Bool::TRUE);
			//Agent->SetLoopDetectionEnabled(enabled == Bool::TRUE);
		}





		DiScenXpWrapper::DiScenXpWrapper()
		{
			AgentName = "Default";
			//Agent = std::make_shared<CyberSystemAgent>();
		}



		void DiScenXpWrapper::ClearActionsCache()
		{
			AvailableActionsCache.clear();
			ForbiddenActionsCache.clear();
			SuggestedActionsCache.clear();
		}


		std::shared_ptr<EnvironmentState> DiScenXpWrapper::LastStateChanges()
		{
			return DiScenFw()->GetLastScenarioState(GetAgentName());
		}


		const char* DiScenXpWrapper::GetInfo(const char* infoId)
		{
			LastInfoCache[infoId] = DiScenFw()->GetSystemInfo(infoId);
			//LastInfoCache[infoId] = Agent->GetSystemInfo(infoId);
			return LastInfoCache[infoId].c_str();
		}


		Bool DiScenXpWrapper::SetConfiguration(const char* config)
		{
			return DiScenFw()->SetSystemConfiguration(config) ? TRUE : FALSE;
		}


		const char* DiScenXpWrapper::GetConfiguration()
		{
			return DiScenFw()->GetSystemConfiguration().c_str();
		}


		const char* DiScenXpWrapper::GetEntityConfiguration(const char* entityId)
		{
			SystemConfigCache = DiScenFw()->GetEntity(entityId)->Configuration;
				//->ReadEntityConfiguration(entityId);
			return SystemConfigCache.c_str();
		}


		Bool DiScenXpWrapper::SetEntityConfiguration(const char* entityId, const char* config)
		{
			return DiScenFw()->ConfigureEntity(entityId,config) ? TRUE : FALSE;
			//return Agent->GetCyberSystem()->WriteEntityConfiguration(entityId, config) ? TRUE : FALSE;
		}


		const char* DiScenXpWrapper::GetCurrentGoal()
		{
			CurrentGoalCache = DiScenFw()->GetCurrentGoal(GetAgentName());
			return CurrentGoalCache.c_str();
		}


		int DiScenXpWrapper::GetGoalsCount()
		{
			GoalNamesCache = DiScenFw()->GetGoals(GetAgentName());
			return (int)GoalNamesCache.size();
		}


		int DiScenXpWrapper::GetGoals(const char** goalNames, int* size)
		{
			GoalNamesCache = DiScenFw()->GetGoals(GetAgentName());
			if (goalNames && size)
			{
				int count = 0;
				for (const std::string& goal : GoalNamesCache)
				{
					if (count >= *size) break;
					goalNames[count] = goal.c_str();
					count++;
				}
				*size = count;
			}
			return (int)GoalNamesCache.size();
		}


		Bool DiScenXpWrapper::RemoveGoal(const char* goalName)
		{
			DiScenFw()->ResetAgent(GetAgentName());
			bool result = DiScenFw()->RemoveGoal(GetAgentName(),goalName);

			return result ? TRUE : FALSE;
		}


		Bool DiScenXpWrapper::SetCurrentGoal(const char* goalName)
		{
			return DiScenFw()->SetCurrentGoal(GetAgentName(),goalName) ? TRUE : FALSE;
		}


		Bool DiScenXpWrapper::AddNewGoal(const char* goalName)
		{
			return DiScenFw()->AddNewGoal(GetAgentName(),goalName) ? TRUE : FALSE;
		}


		Bool DiScenXpWrapper::LoadCurrentExperience(const char* filePath)
		{
			return DiScenFw()->LoadExperience(GetAgentName(),filePath) ? TRUE : FALSE;
		}

		Bool DiScenXpWrapper::SaveCurrentExperience(const char* filePath)
		{
			return DiScenFw()->SaveExperience(GetAgentName(),filePath) ? TRUE : FALSE;
		}


		Result DiScenXpWrapper::TrainAgent(Bool updateXp, Bool agentLearning)
		{
			ClearActionsCache();
			//if (LastActionResult != ActionResult::IN_PROGRESS)
			//{
			//	DiScenFw()->SetLevel(ExperienceLevel::ASSISTANT);
			//}
			AgentMode mode = AgentMode::JUST_ACT;
			if (agentLearning) mode = (AgentMode)(mode|AgentMode::LEARN);
			if (DeadlockDetectionFlag) mode = (AgentMode)(mode|AgentMode::LOOP_DETECTION);
			LastActionResult = DiScenFw()->Train(GetAgentName(),updateXp == Bool::TRUE,mode).Result;
			return (Result)LastActionResult;
		}


		const std::string& DiScenXpWrapper::GetAgentName()
		{
			if (!DiScenFw()->AgentExists(AgentName))
			{
				DiScenFw()->CreateAgent(AgentName);
			}
			return AgentName;
		}


		void DiScenXpWrapper::ExportAction(const Action& action, ActionData& actionData)
		{
			actionData.ActionId = action.TypeId.c_str();
			actionData.ParamsCount = (int)action.Params.size();
			int paramsCount = (int)action.Params.size();
			// TODO: free allocated memory (warning: char* pointers refer to std::string internal data)
			actionData.Params = new const char*[paramsCount];
			for (int i = 0; i < paramsCount; i++)
			{
				actionData.Params[i] = action.Params[i].c_str();
			}
		}

	} // namespace xp
} // namespace discenfw


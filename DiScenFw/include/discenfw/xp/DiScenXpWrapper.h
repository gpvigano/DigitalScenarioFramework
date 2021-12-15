//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "DiScenXp.h"
#include "discenfw/xp/CyberSystemAgent.h"

namespace discenfw
{
	namespace xp
	{
		/*!
		Wrapper remapping DiScenXp API to C functions for interoperability implementation (DLL/DSL).

		Its methods are based on C types and used to build a C-style interface
		that can be used in any platform (assuming suitable binaries are provided).
		@note This remapping, compared to the original API, provides less fetures with a different interface.
		@see DiScenXp.h, CyberSystemAgent
		*/
		class DiScenXpWrapper
		{
		public:
			static std::shared_ptr<DiScenXpWrapper> GetInstance();
			static bool Created() { return Instance ? true : false; }
			static void DestroyInstance();

			//std::shared_ptr<CyberSystemAgent> Agent;

			Result GetLastActionResult() { return (Result)LastActionResult; }

			void SetCyberSystemDllDirectory(const char* dllDirPath);
			Bool LoadCyberSystem(const char* dllName);

			void NewEpisode();
			void ClearAllExperiences();
			void ClearCurrentExperience();

			std::shared_ptr<EnvironmentState> GetLastState();

			Result TakeAction(const char* actionId, const char** params, int paramsCount, Bool updateXp);
			const char* GetEntityProperty(const char* entityId, const char* propertyId);
			Bool CheckEntityProperty(const char* entityId,
				const char* propertyId, const char* propertyValue);
			Bool EntityHasRelationship(const char* entityId, const char* RelationshipId);
			int GetEntityPropertiesCount(const char* entityId);
			int GetEntityProperties(const char* entityId, PropertyData* properties, int* size);
			int GetScenarioEntitiesCount();
			int GetScenarioEntities(const char** entityId, int* size);
			int GetChangedEntitiesCount();
			int GetChangedEntities(const char** entityId, int* size);
			int GetEntityRelationshipsCount(const char* entityId);
			int GetEntityRelationships(const char* entityId, RelationshipData* relationships, int* size);

			const char* GetEntityStateType(const char* entityId);
			int GetPossiblePropertiesCount(const char* entityTypeId);
			Bool GetPossibleProperties(const char* entityTypeId, const char** propertyIds, int* size);
			int GetPossiblePropertyValuesCount(const char* entityTypeId, const char* propertyId);
			Bool GetPossiblePropertyValues(const char* entityTypeId,
				const char* propertyId, const char** values, int* size);

			int GetActionParamsCount(ActionData* actionPtr);
			void GetAction(ActionData* actionPtr, const char** actionId, const char** paramsArray, int* paramsCount);

			int GetAvailableActionsCount();
			int GetAvailableActions(ActionData** availableActionsData, int* size);
			int GetForbiddenActionsCount();
			int GetForbiddenActions(ActionData** forbiddenActionsData, int* size);
			int GetSuggestedActionsCount();
			int GetSuggestedActions(ActionData** suggestedActionsData, int* size);
			void AddSuccessCondition(const char* entityId, const char* propertyId, const char* propertyValue);
			void ResetSuccessCondition();
			int GetSuccessConditionsCount();
			Bool GetSuccessConditions(ConditionData* successConditionsData, int* size);
			void SetDeadlockDetection(Bool enabled);

			Result TrainAgent(Bool updateXp, Bool agentLearning);

			void UpdateActionsCache();
			void ClearActionsCache();

			std::shared_ptr<EnvironmentState> LastStateChanges();
			const char* GetInfo(const char* infoId);

			Bool SetConfiguration(const char* config);
			const char* GetConfiguration();
			const char* GetEntityConfiguration(const char* entityId);
			Bool SetEntityConfiguration(const char* entityId, const char* config);

			const char* GetCurrentGoal();
			int GetGoalsCount();
			int GetGoals(const char** goalNames, int* size);
			Bool RemoveGoal(const char* goalName);
			Bool SetCurrentGoal(const char* goalName);
			Bool AddNewGoal(const char* goalName);

			Bool LoadCurrentExperience(const char* filePath);
			Bool SaveCurrentExperience(const char* filePath);

		private:
			struct ConditionDataCache
			{
				std::string EntityId;
				std::string PropertyId;
				std::string PropertyValue;
			};

			static std::shared_ptr<DiScenXpWrapper> Instance;

			std::string CyberSystemDllDirectory;
			std::string CyberSystemDllName;
			std::string AgentName;
			bool DeadlockDetectionFlag = true;

			ActionResult LastActionResult = ActionResult::IN_PROGRESS;

			// Persistent cache for exported data

			std::vector<ActionRef> AvailableActionsCache;
			std::vector<Action> ForbiddenActionsCache;
			std::vector<Action> SuggestedActionsCache;
			std::vector<ActionData> AvailableActionsData;
			std::vector<ActionData> ForbiddenActionsData;
			std::vector<ActionData> SuggestedActionsData;
			std::map<std::string, std::string> LastInfoCache;
			std::string CurrentGoalCache;
			std::string SystemConfigCache;
			std::string EntityStateTypeNameBuffer;
			std::vector<std::string> GoalNamesCache;
			std::vector<std::string> PropertyIdsCache;
			std::vector<std::string> PropertyValuesCache;
			std::vector<ConditionDataCache> SuccessConditionsData;

			DiScenXpWrapper();

			const std::string& GetAgentName();
			void ExportAction(const Action& action, ActionData& actionData);
		};
	}

}

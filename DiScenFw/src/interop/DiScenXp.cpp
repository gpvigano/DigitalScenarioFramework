//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#ifdef DISCENFW_EXPORT
#include "DiScenXp.h"
#include "discenfw/xp/DiScenXpWrapper.h"
#include "discenfw/interop/CyberSystemLink.h"
#include "discenfw/DigitalScenarioFramework.h"

using namespace discenfw;
using namespace discenfw::xp;

extern "C"
{
	Bool LoadCyberSystem(const char* dllName)
	{
		return DiScenXpWrapper::GetInstance()->LoadCyberSystem(dllName);
	}

	Bool IsCyberSystemLoaded()
	{
		return DiScenFw()->IsCyberSystemLoaded()?TRUE:FALSE;
	}

	int GetGoalsCount()
	{
		return DiScenXpWrapper::GetInstance()->GetGoalsCount();
	}

	int GetGoals(const char** goalNames, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetGoals(goalNames, size);
	}

	const char* GetCurrentGoal()
	{
		return DiScenXpWrapper::GetInstance()->GetCurrentGoal();
	}

	Bool SetCurrentGoal(const char* goalName)
	{
		return DiScenXpWrapper::GetInstance()->SetCurrentGoal(goalName);
	}

	Bool AddNewGoal(const char* goalName)
	{
		return DiScenXpWrapper::GetInstance()->AddNewGoal(goalName);
	}

	Bool RemoveGoal(const char* goalName)
	{
		return DiScenXpWrapper::GetInstance()->RemoveGoal(goalName);
	}

	Bool LoadCurrentExperience(const char* filePath)
	{
		return DiScenXpWrapper::GetInstance()->LoadCurrentExperience(filePath);
	}

	Bool SaveCurrentExperience(const char* filePath)
	{
		return DiScenXpWrapper::GetInstance()->SaveCurrentExperience(filePath);
	}

	void ClearAllExperiences()
	{
		DiScenXpWrapper::GetInstance()->ClearAllExperiences();
	}

	void ClearCurrentExperience()
	{
		DiScenXpWrapper::GetInstance()->ClearCurrentExperience();
	}

	void NewEpisode()
	{
		DiScenXpWrapper::GetInstance()->NewEpisode();
	}

	Result LastResult()
	{
		return DiScenXpWrapper::GetInstance()->GetLastActionResult();
	}


	Result TakeAction(const char* actionId, const char** params, int paramsCount, Bool updateXp)
	{
		return DiScenXpWrapper::GetInstance()->TakeAction(actionId, params, paramsCount, updateXp);
	}

	const char* GetEntityProperty(const char* entityId, const char* propertyId)
	{
		return DiScenXpWrapper::GetInstance()->GetEntityProperty(entityId, propertyId);
	}

	Bool CheckEntityProperty(const char* entityId,
		const char* propertyId, const char* propertyValue)
	{
		return DiScenXpWrapper::GetInstance()->CheckEntityProperty(entityId, propertyId, propertyValue);
	}

	Bool EntityHasRelationship(const char* entityId, const char* RelationshipId)
	{
		return DiScenXpWrapper::GetInstance()->EntityHasRelationship(entityId, RelationshipId);
	}

	int GetEntityPropertiesCount(const char* entityId)
	{
		return DiScenXpWrapper::GetInstance()->GetEntityPropertiesCount(entityId);
	}

	int GetEntityProperties(const char* entityId, PropertyData* properties, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetEntityProperties(entityId, properties, size);
	}

	int GetScenarioEntitiesCount()
	{
		return DiScenXpWrapper::GetInstance()->GetScenarioEntitiesCount();
	}

	int GetScenarioEntities(const char** entityId, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetScenarioEntities(entityId, size);
	}

	int GetChangedEntitiesCount()
	{
		return DiScenXpWrapper::GetInstance()->GetChangedEntitiesCount();
	}

	int GetChangedEntities(const char** entityId, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetChangedEntities(entityId, size);
	}

	int GetEntityRelationshipsCount(const char* entityId)
	{
		return DiScenXpWrapper::GetInstance()->GetEntityRelationshipsCount(entityId);
	}
	const char* GetEntityStateType(const char* entityId)
	{
		return DiScenXpWrapper::GetInstance()->GetEntityStateType(entityId);
	}

	int GetPossiblePropertiesCount(const char* entityTypeId)
	{
		return DiScenXpWrapper::GetInstance()->GetPossiblePropertiesCount(entityTypeId);
	}

	Bool GetPossibleProperties(const char* entityTypeId, const char** propertyIds, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetPossibleProperties(entityTypeId, propertyIds, size);
	}

	int GetPossiblePropertyValuesCount(const char* entityTypeId, const char* propertyId)
	{
		return DiScenXpWrapper::GetInstance()->GetPossiblePropertyValuesCount(entityTypeId, propertyId);
	}

	Bool GetPossiblePropertyValues(const char* entityTypeId, const char* propertyId, const char** values, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetPossiblePropertyValues(entityTypeId, propertyId, values, size);
	}

	int GetEntityRelationships(const char* entityId, RelationshipData* relationships, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetEntityRelationships(entityId, relationships, size);
	}

	int GetActionParamsCount(ActionData* actionPtr)
	{
		return DiScenXpWrapper::GetInstance()->GetActionParamsCount(actionPtr);
	}

	void GetAction(ActionData* actionPtr, const char** actionId, const char** paramsArray, int* paramsCount)
	{
		DiScenXpWrapper::GetInstance()->GetAction(actionPtr, actionId, paramsArray, paramsCount);
	}

	int GetAvailableActionsCount()
	{
		return DiScenXpWrapper::GetInstance()->GetAvailableActionsCount();
	}

	int GetForbiddenActionsCount()
	{
		return DiScenXpWrapper::GetInstance()->GetForbiddenActionsCount();
	}

	int GetSuggestedActionsCount()
	{
		return DiScenXpWrapper::GetInstance()->GetSuggestedActionsCount();
	}

	int GetAvailableActions(ActionData** availableActionsData, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetAvailableActions(availableActionsData, size);
	}

	int GetForbiddenActions(ActionData** forbiddenActionsData, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetForbiddenActions(forbiddenActionsData, size);
	}

	int GetSuggestedActions(ActionData** suggestedActionsData, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetSuggestedActions(suggestedActionsData, size);
	}

	const char* GetInfo(const char* infoId)
	{
		return DiScenXpWrapper::GetInstance()->GetInfo(infoId);
	}

	const char* GetConfiguration()
	{
		return DiScenXpWrapper::GetInstance()->GetConfiguration();
	}

	Bool SetConfiguration(const char* config)
	{
		return DiScenXpWrapper::GetInstance()->SetConfiguration(config) ? TRUE : FALSE;
	}

	const char* GetEntityConfiguration(const char* entityId)
	{
		return DiScenXpWrapper::GetInstance()->GetEntityConfiguration(entityId);
	}

	Bool SetEntityConfiguration(const char* entityId, const char* config)
	{
		return DiScenXpWrapper::GetInstance()->SetEntityConfiguration(entityId, config) ? TRUE : FALSE;
	}

	void AddSuccessCondition(const char* entityId, const char* propertyId, const char* propertyValue)
	{
		return DiScenXpWrapper::GetInstance()->AddSuccessCondition(entityId, propertyId, propertyValue);
	}

	void ResetSuccessCondition()
	{
		return DiScenXpWrapper::GetInstance()->ResetSuccessCondition();
	}

	int GetSuccessConditionsCount()
	{
		return DiScenXpWrapper::GetInstance()->GetSuccessConditionsCount();
	}

	Bool GetSuccessConditions(ConditionData* successConditionsData, int* size)
	{
		return DiScenXpWrapper::GetInstance()->GetSuccessConditions(successConditionsData, size);
	}

	Result TrainAgent(Bool updateXp, Bool agentLearning)
	{
		return DiScenXpWrapper::GetInstance()->TrainAgent(updateXp, agentLearning);
	}

	void SetDeadlockDetection(Bool enabled)
	{
		return DiScenXpWrapper::GetInstance()->SetDeadlockDetection(enabled);
	}

	void DeinitializeXp()
	{
		DiScenXpWrapper::GetInstance()->ClearAllExperiences();
		DiScenFw()->ResetXp();
		//ClearAllExperiences();
		//discenfw::xp::DiScenXpWrapper::DestroyInstance();
		//discenfw::LogMessage(DEBUG, "Deinitialized.", "DiScenFw");
	}
}


#endif

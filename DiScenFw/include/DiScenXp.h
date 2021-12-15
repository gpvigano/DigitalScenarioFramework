/*
--------------------------------------------------------------------
Digital Scenario Framework
by Giovanni Paolo Vigano', 2021
--------------------------------------------------------------------

Distributed under the MIT Software License.
See http://opensource.org/licenses/MIT
*/

#pragma once

#include "DiScenFwConfig.h"


#ifdef __cplusplus
extern "C"
{
#endif

	/* @cond DOXYGEN_EXCLUDE */
#ifndef BOOL
	enum Bool
	{
		FALSE = 0,
		TRUE = 1,
	};
#define BOOL Bool
#endif
	/* @endcond */

	/*!
	@addtogroup Interoperability C-Style interface for interoperability
	@{
	*/

	/*!
	@addtogroup XpInteroperability Experience management
	@{
	*/


	/*! C-compliant version for discenfw::xp::ActionResult */
	enum Result
	{
		IN_PROGRESS = 0, /*< Task still in progress. */
		FAILED, /*< Failure, something went wrong. */
		SUCCEEDED, /*< Success, task completed. */
		DEADLOCK, /*< Deadlock, task came to a deadlock. */
		DENIED /*< Action denied, unable to take the action. */
	};

	/*!
	These structures contain C-compliant types to map C++ structures to C structs.
	@name C-compliant types
	@{
	*/

	/*! C-compliant version for discenfw::xp::PropertyCondition */
	struct PropertyData
	{
		const char* PropertyId;
		const char* PropertyValue;
	};


	/*! C-compliant version for discenfw::xp::Condition */
	struct ConditionData
	{
		const char* EntityId;
		const char* PropertyId;
		const char* PropertyValue;
	};


	/*! C-compliant version for discenfw::xp::EntityState::Relationships */
	struct RelationshipData
	{
		const char* RelationshipId;
		const char* RelatedEntityId;
		const char* RelatedEndPoint;
	};


	/*! C-compliant version for discenfw::xp::Action */
	struct ActionData
	{
		const char* ActionId;
		const char** Params;
		int ParamsCount;
	};

	/*!@} */

	/*!
	Output memory buffers used by these functions can be lost at each following call.
	Copy them soon after each function call.

	The functions that fill an array of pointers need the size of the array as input.
		You should call the Get*Count() functions before calling these functions to get the real size.
		The given size must contain the size of the given array,
		the array is filled with up to size elements,
		the given size is then changed to a lesser value if the required actions are less
		than the given size.
		These functions return the actual number of actions in the internal list.

	@name Wrapper functions directly calling DiScenXp methods.
	@{
	*/

	DISCENFW_API Bool LoadCyberSystem(const char* dllName);
	DISCENFW_API Bool IsCyberSystemLoaded();

	DISCENFW_API int GetGoalsCount();
	DISCENFW_API int GetGoals(const char** goalNames, int* size);
	DISCENFW_API const char* GetCurrentGoal();
	DISCENFW_API Bool SetCurrentGoal(const char* goalName);
	DISCENFW_API Bool AddNewGoal(const char* goalName);
	DISCENFW_API Bool RemoveGoal(const char* goalName);

	DISCENFW_API Bool LoadCurrentExperience(const char* filePath);
	DISCENFW_API Bool SaveCurrentExperience(const char* filePath);
	DISCENFW_API void ClearAllExperiences();
	DISCENFW_API void ClearCurrentExperience();
	DISCENFW_API void NewEpisode();
	DISCENFW_API Result LastResult();
	DISCENFW_API Result TakeAction(const char* actionId, const char** params, int paramsCount, Bool updateXp);
	DISCENFW_API const char* GetEntityProperty(const char* entityId, const char* propertyId);
	DISCENFW_API Bool CheckEntityProperty(const char* entityId, const char* propertyId, const char* propertyValue);
	DISCENFW_API Bool EntityHasRelationship(const char* entityId, const char* RelationshipId);
	DISCENFW_API int GetScenarioEntitiesCount();
	DISCENFW_API int GetScenarioEntities(const char** entityId, int* size);
	DISCENFW_API int GetChangedEntitiesCount();
	DISCENFW_API int GetChangedEntities(const char** entityId, int* size);
	DISCENFW_API int GetEntityPropertiesCount(const char* entityId);
	DISCENFW_API int GetEntityProperties(const char* entityId, PropertyData* properties, int* size);
	DISCENFW_API int GetEntityRelationshipsCount(const char* entityId);
	DISCENFW_API int GetEntityRelationships(const char* entityId, RelationshipData* relationships, int* size);

	DISCENFW_API const char* GetEntityStateType(const char* entityId);
	DISCENFW_API int GetPossiblePropertiesCount(const char* entityTypeId);
	DISCENFW_API Bool GetPossibleProperties(const char* entityTypeId, const char** propertyIds, int* size);
	DISCENFW_API int GetPossiblePropertyValuesCount(const char* entityTypeId, const char* propertyId);
	DISCENFW_API Bool GetPossiblePropertyValues(const char* entityTypeId, const char* propertyId, const char** values, int* size);

	DISCENFW_API int GetActionParamsCount(ActionData* actionPtr);
	DISCENFW_API void GetAction(ActionData* actionPtr, const char** actionId, const char** paramsArray, int* paramsCount);

	DISCENFW_API const char* GetInfo(const char* infoId);
	DISCENFW_API const char* GetConfiguration();
	DISCENFW_API Bool SetConfiguration(const char* config);
	DISCENFW_API const char* GetEntityConfiguration(const char* entityId);
	DISCENFW_API Bool SetEntityConfiguration(const char* entityId, const char* config);

	DISCENFW_API void AddSuccessCondition(const char* entityId, const char* propertyId, const char* propertyValue);
	DISCENFW_API void ResetSuccessCondition();
	DISCENFW_API int GetSuccessConditionsCount();
	DISCENFW_API Bool GetSuccessConditions(ConditionData* successConditionsData, int* size);

	/*! Train the current agent (see CyberSystemAgent). */
	DISCENFW_API Result TrainAgent(Bool updateXp, Bool agentLearning);

	/*! Enable or disable deadlock detection. */
	DISCENFW_API void SetDeadlockDetection(Bool enabled);

	/*! @}  */

	/*!
	The following functions fill an array of ActionData* pointers.

	The given size must contain the size of the given array,
	the array is filled with up to size elements,
	the given size is then changed to a lesser value if the required actions are less
	than the given size.
	These functions return the actual number of actions in the internal list.
	You should call the Get*Count() functions before calling these functions to get the real size.
	*/
	/*!
	@name Actions getters.
	@{
	*/

	DISCENFW_API int GetAvailableActionsCount();
	DISCENFW_API int GetForbiddenActionsCount();
	DISCENFW_API int GetSuggestedActionsCount();

	DISCENFW_API int GetAvailableActions(ActionData** availableActionsData, int* size);
	DISCENFW_API int GetForbiddenActions(ActionData** forbiddenActionsData, int* size);
	DISCENFW_API int GetSuggestedActions(ActionData** suggestedActionsData, int* size);

	/*! @}  */
	DISCENFW_API void DeinitializeXp();
	/*! @}  */
	/*! @}  */

#ifdef __cplusplus
}
#endif


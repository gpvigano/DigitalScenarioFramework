/*
--------------------------------------------------------------------
Digital Scenario Framework
by Giovanni Paolo Vigano', 2021
--------------------------------------------------------------------

Distributed under the MIT Software License.
See http://opensource.org/licenses/MIT
*/

/*!
@brief C-style interface for the scenario API.
@file DiScenAPI.h
@author Giovanni Paolo Vigano'
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
		@addtogroup ScenInteroperability Scenario management
		@{
		*/

		/*!
		These structures contain C-compliant types to map C++ structures to C structs.
		@name C-compliant types
		@{
		*/

		/*! C-compliant version for discenfw::AssetReference::SourceType */
	enum AssetSourceType
	{
		SCENE = 0,
		PROJECT,
		EXTERNAL,
		UNDEFINED
	};


	/*! C-compliant version for discenfw::Vector3D */
	struct Vector3f
	{
		float Right;
		float Forward;
		float Up;
	};


	/*! C-compliant version for discenfw::LocalTransform */
	struct TransformData
	{
		Vector3f RightAxis;
		Vector3f ForwardAxis;
		Vector3f UpAxis;
		Vector3f Origin;
		Vector3f Scale;
		const char* ParentId;
	};


	/*! C-compliant version for discenfw::AssetReference */
	struct AssetData
	{
		int Source = UNDEFINED;
		const char* Catalog;
		const char* AssetType;
		const char* Uri;
		const char* PartId;
	};


	/*! C-compliant version for discenfw::Entity */
	struct EntityData
	{
		const char* Class;
		const char* Identifier;
		const char* Type;
		const char* Category;
		const char* Description;
		const char* Configuration;
		AssetData Asset;
	};


	/*! C-compliant version for discenfw::Element */
	struct ElementData
	{
		EntityData Entity;
		TransformData Transform;
	};

	/*! @} */


	/*!
	Callback function used to display messages.

	Severity can be:
	@li 	0 = @c DEBUG
	@li 	1 = @c VERBOSE
	@li 	2 = @c LOG
	@li 	3 = @c WARNING
	@li 	4 = @c ERROR
	@li 	5 = @c FATAL
	*/
	typedef void(__stdcall *DisplayMessageCallback)(
		int severity,
		const char* message, const char* category,
		Bool onConsole, Bool onScreen, const char* msgTag);

	typedef void(__stdcall *EventCallback)();

	typedef void(__stdcall *EventFloatCallback)(float floatParam);

	typedef void(__stdcall *SyncScenarioCallback)();

	typedef void(__stdcall *SyncSceneCallback)();

	typedef const char*(__stdcall *ProjectDirCallback)();

	typedef void(__stdcall *SyncElementTransformCallback)(const char* elemId,
		TransformData* sceneTransform);

	typedef void(__stdcall *SyncSceneObjectTransformCallback)(const char* elemId,
		TransformData* scenarioTransform);

	typedef void(__stdcall *LerpElementTransformCallback)(const char* elemId,
		TransformData* transform1, TransformData* transform2, float trim);


	DISCENFW_API void RegisterDisplayMessageCallback(DisplayMessageCallback);
	DISCENFW_API void RegisterProjectDirCallback(ProjectDirCallback);
	DISCENFW_API void RegisterSyncScenarioCallback(SyncScenarioCallback);
	DISCENFW_API void RegisterSyncSceneCallback(SyncSceneCallback);
	DISCENFW_API void RegisterSyncElementTransformCallback(SyncElementTransformCallback);
	DISCENFW_API void RegisterSyncSceneObjectTransformCallback(SyncSceneObjectTransformCallback);
	DISCENFW_API void RegisterLerpElementTransformCallback(LerpElementTransformCallback);

	DISCENFW_API void ClearScenario();
	DISCENFW_API Bool LoadScenario(const char* path, Bool syncVE);
	DISCENFW_API Bool SaveScenario(const char* path);
	DISCENFW_API void UpdateScenario(float timeDelta);
	DISCENFW_API int GetEntitiesCount();
	DISCENFW_API int GetEntities(EntityData* entities, int* size);
	DISCENFW_API int GetElementsCount();
	DISCENFW_API int GetElements(ElementData* elements, int* size);
	DISCENFW_API Vector3f GetElementLocation(const char* id);
	DISCENFW_API void AddElements(ElementData* elements, int size);
	DISCENFW_API void AddElement(ElementData* elementData);
	DISCENFW_API Bool DeleteElement(const char* elementId);

	DISCENFW_API void ClearSimulation();
	DISCENFW_API Bool LoadSimulation(const char* path);
	DISCENFW_API Bool SaveSimulation(const char* path);
	DISCENFW_API void UpdateSimulation(float timeDelta);

	DISCENFW_API Bool ValidSimulation();
	DISCENFW_API Bool SimulationStarted();
	DISCENFW_API void PlaySimulation();
	DISCENFW_API void PauseSimulation();
	DISCENFW_API void StopSimulation();
	DISCENFW_API void SetSimulationProgress(double progress);
	DISCENFW_API double ComputeSimulationProgress();

	DISCENFW_API void RegisterSimulationLoadedCallback(EventCallback eventCallback);
	DISCENFW_API void RegisterSimulationUpdatedCallback(EventCallback eventCallback);
	DISCENFW_API void RegisterSimulationPlayCallback(EventCallback eventCallback);
	DISCENFW_API void RegisterSimulationPauseCallback(EventCallback eventCallback);
	DISCENFW_API void RegisterSimulationStopCallback(EventCallback eventCallback);
	DISCENFW_API void RegisterSimulationTimeChangedCallback(EventFloatCallback eventCallback);
	DISCENFW_API const char* GetSimulationDateTimeAsString();

	DISCENFW_API void Deinitialize();

	/*! @} */

	/*! @} */

#ifdef __cplusplus
}
#endif



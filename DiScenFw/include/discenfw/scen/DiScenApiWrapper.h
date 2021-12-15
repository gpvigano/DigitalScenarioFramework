//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "DiScenAPI.h"
#include "discenfw/scen/ScenarioData.h"
#include "discenfw/scen/ScenarioManager.h"
#include <discenfw/sim/SimulationManager.h>
#include <gpvulc/time/DateTimeUtil.h>

namespace discenfw
{
	namespace scen
	{
		/*!
		Wrapper with methods based on C types, used to build a C-style interface
		that can be used in any platform (assuming suitable binaries are provided).
		@see DiScenAPI.h
		*/
		class DiScenApiWrapper
		{
		public:
			static std::shared_ptr<DiScenApiWrapper> GetInstance();
			static void DestroyInstance();

			std::shared_ptr<ScenarioManager> Scenario() { return _ScenarioManager; }
			std::shared_ptr<sim::SimulationManager> Simulation() { return _SimulationManager; }

			//void RegisterTakeScreenshotCallback(TakeScreenshotCallback takeScreenshotCallback);
			void RegisterDisplayMessageCallback(DisplayMessageCallback displayMessageCallback);
			void RegisterProjectDirCallback(ProjectDirCallback projectDirCallback);
			void RegisterSyncScenarioCallback(SyncScenarioCallback syncScenarioCallback);
			void RegisterSyncSceneCallback(SyncSceneCallback syncSceneCallback);
			void RegisterSyncElementTransformCallback(SyncElementTransformCallback);
			void RegisterSyncSceneObjectTransformCallback(SyncSceneObjectTransformCallback);
			void RegisterLerpElementTransformCallback(LerpElementTransformCallback lerpElementTransformCallback);

			// event actions SimulationLoaded, SimulationUpdated, ...

			void RegisterSimulationLoadedCallback(EventCallback eventCallback);
			void RegisterSimulationUpdatedCallback(EventCallback eventCallback);
			void RegisterSimulationPlayCallback(EventCallback eventCallback);
			void RegisterSimulationPauseCallback(EventCallback eventCallback);
			void RegisterSimulationStopCallback(EventCallback eventCallback);
			void RegisterSimulationTimeChangedCallback(EventFloatCallback eventCallback);

			void ClearScenario();
			Bool LoadScenario(const char* path, Bool syncVE) { return _ScenarioManager->LoadScenarioJson(path,syncVE==TRUE) ? TRUE : FALSE; }
			Bool SaveScenario(const char* path) { return _ScenarioManager->SaveScenarioJson(path) ? TRUE : FALSE; }
			const char* GetScenarioJson();
			Bool SetScenarioJson(const char* jsonText);
			void UpdateScenario(float timeDelta) { _ScenarioManager->UpdateScenario(timeDelta); }
			int GetEntitiesCount() { return (int)_ScenarioManager->GetData()->GetEntities().size(); }
			int GetEntities(EntityData* entities, int* size);
			int GetElementsCount();
			int GetElements(ElementData* elements, int* size);
			void AddElements(ElementData* elements, int size);
			void AddElement(ElementData* elementData);
			Bool DeleteElement(const char* elementId);

			Vector3f GetElementLocation(const char* id);

			void ClearSimulation();
			Bool LoadSimulation(const char* path) { return _SimulationManager->LoadHistoryJson(path) ? TRUE : FALSE; }
			Bool SaveSimulation(const char* path) { return _SimulationManager->SaveHistoryJson(path) ? TRUE : FALSE; }
			const char* GetSimulationJson();
			Bool SetSimulationJson(const char* jsonText);
			void UpdateSimulation(float timeDelta) { _SimulationManager->UpdateSimulation(timeDelta); }

			Bool ValidSimulation()
			{
				return _SimulationManager->Simulation->ValidSimulation()?TRUE:FALSE;
			}

			void PlaySimulation()
			{
				_SimulationManager->Simulation->PlaySimulation();
			}

			void PauseSimulation()
			{
				_SimulationManager->Simulation->PauseSimulation();
			}

			void StopSimulation()
			{
				_SimulationManager->Simulation->StopSimulation();
			}

			Bool SimulationStarted()
			{
				return _SimulationManager->Simulation->SimulationStarted()?TRUE:FALSE;
			}

			void SetSimulationProgress(double progress)
			{
				_SimulationManager->Simulation->SetSimulationProgress(progress);
			}

			double ComputeSimulationProgress()
			{
				return _SimulationManager->Simulation->ComputeSimulationProgress();
			}

			const char* GetSimulationDateTimeAsString()
			{
				DateTimeStringBuffer = gpvulc::DateTimeToString(_SimulationManager->Simulation->GetSimulationDateTime());
				return DateTimeStringBuffer.c_str();
			}

		private:

			static std::shared_ptr<DiScenApiWrapper> Instance;
			std::shared_ptr<ScenarioManager> _ScenarioManager;
			std::shared_ptr<sim::SimulationManager> _SimulationManager;

			//TakeScreenshotCallback _TakeScreenshotCallback = nullptr;
			DisplayMessageCallback _DisplayMessageCallback = nullptr;
			ProjectDirCallback _ProjectDirCallback = nullptr;
			SyncElementTransformCallback _SyncElementTransformCallback = nullptr;
			SyncSceneObjectTransformCallback _SyncSceneObjectTransformCallback = nullptr;
			LerpElementTransformCallback _LerpElementTransformCallback = nullptr;

			std::string ScenarioJsonBuffer;
			std::string SimulationJsonBuffer;
			std::string ErrorMessageBuffer;
			std::string DateTimeStringBuffer;

			DiScenApiWrapper();

			void CopyEntityData(const std::shared_ptr<Entity>& entity, EntityData& entityData);
			void CopyTransformData(const LocalTransform& transform, TransformData& transformData);
			void CopyLocalTransform(const TransformData& transformData, LocalTransform& transform);
			Vector3D ToVector3D(const Vector3f& vec);
			Vector3f ToVector3f(const Vector3D& vec);
		};


		inline Vector3D DiScenApiWrapper::ToVector3D(const Vector3f& vec)
		{
			return Vector3D(vec.Right, vec.Forward, vec.Up);
		}


		inline Vector3f DiScenApiWrapper::ToVector3f(const Vector3D& vec)
		{
			Vector3f v3f;
			v3f.Right = vec.Right;
			v3f.Forward = vec.Forward;
			v3f.Up = vec.Up;
			return v3f;
		}
	}
}


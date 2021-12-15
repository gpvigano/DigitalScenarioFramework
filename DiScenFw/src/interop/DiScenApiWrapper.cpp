//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/DiScenApiWrapper.h>
#include <discenfw/ve/VirtualEnvironmentAPI.h>
#include <discenfw/ve/VeManager.h>
#include <discenfw/util/MessageLog.h>


// TODO: route calls to DiScenFw()

namespace discenfw
{
	using namespace ve;

	namespace scen
	{
		std::shared_ptr<DiScenApiWrapper> DiScenApiWrapper::Instance;


		std::shared_ptr<DiScenApiWrapper> DiScenApiWrapper::GetInstance()
		{
			if (!Instance)
			{
				Instance = std::shared_ptr<DiScenApiWrapper>(new DiScenApiWrapper());
			}
			return Instance;
		}


		void DiScenApiWrapper::DestroyInstance()
		{
			if (Instance)
			{
				Instance = nullptr;
			}
		}


		void DiScenApiWrapper::RegisterDisplayMessageCallback(DisplayMessageCallback displayMessageCallback)
		{
			_DisplayMessageCallback = displayMessageCallback;
			discenfw::SetLogDisplayFunction( [&](int severity,
				const std::string& message, const std::string& category,
				bool onConsole, bool onScreen, const std::string& msgTag)
			{
				_DisplayMessageCallback(severity,
					message.c_str(), category.c_str(),
					onConsole ? TRUE : FALSE, onScreen ? TRUE : FALSE, msgTag.c_str());
			});
		}


		void DiScenApiWrapper::RegisterProjectDirCallback(ProjectDirCallback projectDirCallback)
		{
			_ProjectDirCallback = projectDirCallback;
			VE()->ProjectDir = [&]() -> std::string
			{
				std::string path(_ProjectDirCallback());
				return path;
			};
		}


		//void DiScenApiWrapper::RegisterTakeScreenshotCallback(TakeScreenshotCallback takeScreenshotCallback)
		//{
		//	_TakeScreenshotCallback = takeScreenshotCallback;
		//	VE()->TakeScreenshot = [&](const std::string& filePath)
		//	{
		//		_TakeScreenshotCallback(filePath.c_str());
		//	};
		//}


		//void DiScenApiWrapper::RegisterLoadScenarioCallback(LoadScenarioCallback loadScenarioCallback)
		//{
		//	_LoadScenarioCallback = loadScenarioCallback;
		//	VE()->LoadScenario = [&](const std::string& path)
		//	{
		//		_LoadScenarioCallback(path.c_str());
		//	};
		//}


		//void DiScenApiWrapper::RegisterSaveScenarioCallback(SaveScenarioCallback saveScenarioCallback)
		//{
		//	_SaveScenarioCallback = saveScenarioCallback;
		//	VE()->SaveScenario = [&](const std::string& path)
		//	{
		//		_SaveScenarioCallback(path.c_str());
		//	};
		//}


		void DiScenApiWrapper::RegisterSyncScenarioCallback(SyncScenarioCallback syncScenarioCallback)
		{
			VE()->SyncScenario = syncScenarioCallback;
		}


		void DiScenApiWrapper::RegisterSyncSceneCallback(SyncSceneCallback syncSceneCallback)
		{
			VE()->SyncScene = syncSceneCallback;
		}


		void DiScenApiWrapper::RegisterSyncElementTransformCallback(SyncElementTransformCallback syncElementTransformCallback)
		{
			_SyncElementTransformCallback = syncElementTransformCallback;
			VE()->SyncElementTransform = [&](
				const std::string& elemId) -> void
			{
				std::shared_ptr<Element> element = _ScenarioManager->GetElementById(elemId);
				if (element)
				{
					TransformData tr1;
					_SyncElementTransformCallback(elemId.c_str(), &tr1);
					CopyLocalTransform(tr1, element->Transform);
				}
			};
		}


		void DiScenApiWrapper::RegisterSyncSceneObjectTransformCallback(SyncSceneObjectTransformCallback syncSceneObjectTransformCallback)
		{
			_SyncSceneObjectTransformCallback = syncSceneObjectTransformCallback;
			VE()->SyncSceneObjectTransform = [&](
				const std::string& elemId) -> void
			{
				std::shared_ptr<Element> element = _ScenarioManager->GetElementById(elemId);
				if (element)
				{
					TransformData tr1;
					CopyTransformData(element->Transform, tr1);
					_SyncSceneObjectTransformCallback(elemId.c_str(), &tr1);
				}
			};
		}


		void DiScenApiWrapper::RegisterLerpElementTransformCallback(LerpElementTransformCallback lerpElementTransformCallback)
		{
			_LerpElementTransformCallback = lerpElementTransformCallback;
			VE()->LerpElementTransform = [&](
				const std::string& elemId,
				const LocalTransform& transform1,
				const LocalTransform& transform2,
				float trim) -> void
			{
				TransformData tr1;
				TransformData tr2;
				CopyTransformData(transform1, tr1);
				CopyTransformData(transform2, tr2);

				_LerpElementTransformCallback(elemId.c_str(), &tr1, &tr2, trim);
			};
		}


		void DiScenApiWrapper::RegisterSimulationLoadedCallback(EventCallback eventCallback)
		{
			_SimulationManager->Simulation->SimulationLoaded = eventCallback;
		}


		void DiScenApiWrapper::RegisterSimulationUpdatedCallback(EventCallback eventCallback)
		{
			_SimulationManager->Simulation->SimulationUpdated = eventCallback;
		}


		void DiScenApiWrapper::RegisterSimulationPlayCallback(EventCallback eventCallback)
		{
			_SimulationManager->Simulation->SimulationPlay = eventCallback;
			if (!_SimulationManager->Simulation->SimulationPlay)
			{
				LogMessage(LOG_ERROR, "Null EventCallback", "DiScenFw|Sim");
			}
		}


		void DiScenApiWrapper::RegisterSimulationPauseCallback(EventCallback eventCallback)
		{
			_SimulationManager->Simulation->SimulationPause = eventCallback;
		}


		void DiScenApiWrapper::RegisterSimulationStopCallback(EventCallback eventCallback)
		{
			_SimulationManager->Simulation->SimulationStop = eventCallback;
		}


		void DiScenApiWrapper::RegisterSimulationTimeChangedCallback(EventFloatCallback eventCallback)
		{
			_SimulationManager->Simulation->SimulationTimeChanged = eventCallback;
		}


		void DiScenApiWrapper::ClearScenario()
		{
			_ScenarioManager->GetData()->ClearEntities();
		}


		const char* DiScenApiWrapper::GetScenarioJson()
		{
			_ScenarioManager->ScenarioToJson(ScenarioJsonBuffer);
			return ScenarioJsonBuffer.c_str();
		}


		Bool DiScenApiWrapper::SetScenarioJson(const char* jsonText)
		{
			if (jsonText == nullptr) return Bool::FALSE;
			ScenarioJsonBuffer = jsonText;
			if(_ScenarioManager->ScenarioFromJson(ScenarioJsonBuffer))
			{
				return Bool::TRUE;
			}
			ErrorMessageBuffer = "Error parsing scenario JSON text.";
			LogMessage(LOG_ERROR, ErrorMessageBuffer, "DiScenFw");
			return Bool::FALSE;
		}


		int DiScenApiWrapper::GetEntities(EntityData* entities, int * size)
		{
			int count = _ScenarioManager->GetEntitiesCount();
			int n = count < *size ? count : *size;
			*size = n;
			for (int i = 0; i < n; i++)
			{
				CopyEntityData(_ScenarioManager->GetData()->Entities[i], entities[i]);
			}
			return count;
		}


		int DiScenApiWrapper::GetElementsCount()
		{
			int count = 0;
			int n = (int)_ScenarioManager->GetEntitiesCount();
			for (int i = 0; i < n; i++)
			{
				if (_ScenarioManager->GetData()->Entities[i]->IsA("Element"))
				{
					count++;
				}
			}
			return count;
		}


		int DiScenApiWrapper::GetElements(ElementData* elements, int * size)
		{
			int count = 0;
			int n = _ScenarioManager->GetEntitiesCount();
			for (int i = 0; i < n; i++)
			{
				const auto& entity = _ScenarioManager->GetData()->Entities[i];
				if (entity->IsA("Element"))
				{
					if (count < *size)
					{
						// entity data
						CopyEntityData(entity, elements[count].Entity);
						// element data
						const auto& element = std::static_pointer_cast<Element>(entity);
						CopyTransformData(element->Transform, elements[count].Transform);
						//elements[count].OwnerId = element->OwnerId.c_str();

						std::string msg;
						msg += element->GetIdentifier() + ": Origin=";
						msg += std::to_string(elements[count].Transform.Origin.Right) + ",";
						msg += std::to_string(elements[count].Transform.Origin.Forward) + ",";
						msg += std::to_string(elements[count].Transform.Origin.Up) + " Scale=";
						msg += std::to_string(elements[count].Transform.Scale.Right) + ",";
						msg += std::to_string(elements[count].Transform.Scale.Forward) + ",";
						msg += std::to_string(elements[count].Transform.Scale.Up);
						LogMessage(LogLevel::LOG_DEBUG, msg, "DiScenFw");
					}
					count++;
				}
			}
			if (*size > count)
			{
				*size = count;
			}
			return count;
		}


		void DiScenApiWrapper::AddElements(ElementData* elements, int size)
		{
			for (int i = 0; i < size; i++)
			{
				AddElement(&elements[i]);
			}
		}


		void DiScenApiWrapper::AddElement(ElementData* elementData)
		{
			if (elementData == nullptr)
			{
				LogMessage(LogLevel::LOG_ERROR, "Wrapper::AddElement - Null element data", "DiScenFw");
				return;
			}
			if (elementData->Entity.Identifier == nullptr)
			{
				LogMessage(LogLevel::LOG_ERROR, "Wrapper::AddElement - Entity with null identifier", "DiScenFw");
				return;
			}
#define GET_VALID_STR(x) x!=nullptr?x:""
			std::shared_ptr<Element> element = std::make_shared<Element>();
			element->Identity.Identifier = elementData->Entity.Identifier;
			element->Identity.Type = GET_VALID_STR(elementData->Entity.Type);
			element->Identity.Category = GET_VALID_STR(elementData->Entity.Category);
			element->Identity.Description = GET_VALID_STR(elementData->Entity.Description);
			element->Configuration = GET_VALID_STR(elementData->Entity.Configuration);
			element->Asset.Source = (AssetReference::SourceType)elementData->Entity.Asset.Source;
			element->Asset.Catalog = GET_VALID_STR(elementData->Entity.Asset.Catalog);
			element->Asset.AssetType = GET_VALID_STR(elementData->Entity.Asset.AssetType);
			element->Asset.Uri = GET_VALID_STR(elementData->Entity.Asset.Uri);
			element->Asset.PartId = GET_VALID_STR(elementData->Entity.Asset.PartId);
			element->Transform.UseCoordSys = true;
			element->Transform.CoordSys.RightAxis = ToVector3D(elementData->Transform.RightAxis);
			element->Transform.CoordSys.ForwardAxis = ToVector3D(elementData->Transform.ForwardAxis);
			element->Transform.CoordSys.UpAxis = ToVector3D(elementData->Transform.UpAxis);
			element->Transform.CoordSys.Origin = ToVector3D(elementData->Transform.Origin);
			element->Transform.Scale = ToVector3D(elementData->Transform.Scale);
			element->Transform.ParentId = GET_VALID_STR(elementData->Transform.ParentId);
#undef GET_VALID_STR
			_ScenarioManager->GetData()->AddEntity(element);
		}


		Bool DiScenApiWrapper::DeleteElement(const char* elementId)
		{
			auto elem = _ScenarioManager->GetElementById(elementId);
			if (elem)
			{
				if (_ScenarioManager->GetData()->RemoveEntity(elem))
				{
					return Bool::TRUE;
				}
			}
			return Bool::FALSE;
		}


		void DiScenApiWrapper::ClearSimulation()
		{
			_SimulationManager->ClearSimulation();
		}


		const char* DiScenApiWrapper::GetSimulationJson()
		{
			_SimulationManager->HistoryToJson(SimulationJsonBuffer);
			return SimulationJsonBuffer.c_str();
		}


		Bool DiScenApiWrapper::SetSimulationJson(const char* jsonText)
		{
			if (jsonText == nullptr) return Bool::FALSE;
			SimulationJsonBuffer = jsonText;
			if (_SimulationManager->HistoryFromJson(SimulationJsonBuffer))
			{
				return Bool::TRUE;
			}
			ErrorMessageBuffer = "Error parsing simulation JSON text";
			LogMessage(LOG_ERROR, ErrorMessageBuffer, "DiScenFw|Sim");
			return Bool::FALSE;
		}


		Vector3f DiScenApiWrapper::GetElementLocation(const char* id)
		{
			std::shared_ptr<Element> elem = _ScenarioManager->GetElementById(id);
			Vector3f vec;
			if (elem)
			{
				if (elem->Transform.UseCoordSys)
				{
					vec.Right = elem->Transform.CoordSys.Origin.Right;
					vec.Forward = elem->Transform.CoordSys.Origin.Forward;
					vec.Up = elem->Transform.CoordSys.Origin.Up;
				}
				else
				{
					vec.Right = elem->Transform.Location.Right;
					vec.Forward = elem->Transform.Location.Forward;
					vec.Up = elem->Transform.Location.Up;
				}
			}
			else
			{
				vec.Right = 0;
				vec.Forward = 0;
				vec.Up = 0;
			}
			return vec;
		}


		DiScenApiWrapper::DiScenApiWrapper()
		{
			_ScenarioManager = std::make_shared<ScenarioManager>();
			_SimulationManager = std::make_shared<sim::SimulationManager>();
			_SimulationManager->SetScenarioData(_ScenarioManager->GetData());
		}


		void DiScenApiWrapper::CopyEntityData(const std::shared_ptr<Entity>& entity, EntityData& entityData)
		{
			entityData.Class = entity->GetClassName();
			entityData.Identifier = entity->Identity.Identifier.c_str();
			entityData.Type = entity->Identity.Type.c_str();
			entityData.Category = entity->Identity.Category.c_str();
			entityData.Description = entity->Identity.Description.c_str();
			entityData.Configuration = entity->Configuration.c_str();
			entityData.Identifier = entity->Identity.Identifier.c_str();
			entityData.Asset.Source = (int)entity->Asset.Source;
			entityData.Asset.Catalog = entity->Asset.Catalog.c_str();
			entityData.Asset.AssetType = entity->Asset.AssetType.c_str();
			entityData.Asset.Uri = entity->Asset.Uri.c_str();
			entityData.Asset.PartId = entity->Asset.PartId.c_str();
		}


		void DiScenApiWrapper::CopyTransformData(const LocalTransform& transform, TransformData& transformData)
		{
			transformData.RightAxis = ToVector3f(transform.CoordSys.RightAxis);
			transformData.ForwardAxis = ToVector3f(transform.CoordSys.ForwardAxis);
			transformData.UpAxis = ToVector3f(transform.CoordSys.UpAxis);
			transformData.Origin = ToVector3f(transform.CoordSys.Origin);
			transformData.Scale = ToVector3f(transform.Scale);
			transformData.ParentId = transform.ParentId.c_str();
		}


		void DiScenApiWrapper::CopyLocalTransform(const TransformData& transformData, LocalTransform& transform)
		{
			transform.CoordSys.RightAxis = ToVector3D(transformData.RightAxis);
			transform.CoordSys.ForwardAxis = ToVector3D(transformData.ForwardAxis);
			transform.CoordSys.UpAxis = ToVector3D(transformData.UpAxis);
			transform.CoordSys.Origin = ToVector3D(transformData.Origin);
			transform.Scale = ToVector3D(transformData.Scale);
			transform.ParentId = transformData.ParentId ? transformData.ParentId : "";
		}

	}
}






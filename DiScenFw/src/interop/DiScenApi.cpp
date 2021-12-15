//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#ifdef DISCENFW_EXPORT
#include <DiScenApi.h>
#include <discenfw/scen/DiScenApiWrapper.h>

//---------------------------------------------------------------------
// C-style interface implementation for interoperability

using namespace discenfw;
using namespace discenfw::scen;
using namespace discenfw::sim;

#define IMPLEMENT_REGISTER_CALLBACK(callbackType)\
	void Register##callbackType(callbackType callback)\
	{\
		DiScenApiWrapper::GetInstance()->Register##callbackType(callback);\
	}

extern "C"
{
	//IMPLEMENT_REGISTER_CALLBACK(TakeScreenshotCallback)
	IMPLEMENT_REGISTER_CALLBACK(ProjectDirCallback)
		IMPLEMENT_REGISTER_CALLBACK(DisplayMessageCallback)
		IMPLEMENT_REGISTER_CALLBACK(SyncScenarioCallback)
		IMPLEMENT_REGISTER_CALLBACK(SyncSceneCallback)
		IMPLEMENT_REGISTER_CALLBACK(SyncElementTransformCallback)
		IMPLEMENT_REGISTER_CALLBACK(SyncSceneObjectTransformCallback)
		IMPLEMENT_REGISTER_CALLBACK(LerpElementTransformCallback)


	void ClearScenario()
	{
		DiScenApiWrapper::GetInstance()->ClearScenario();
	}

	Bool LoadScenario(const char* path, Bool syncVE)
	{
		return DiScenApiWrapper::GetInstance()->LoadScenario(path, syncVE);
	}

	Bool SaveScenario(const char* path)
	{
		return DiScenApiWrapper::GetInstance()->SaveScenario(path);
	}

	void UpdateScenario(float timeDelta)
	{
		DiScenApiWrapper::GetInstance()->UpdateScenario(timeDelta);
	}

	int GetEntitiesCount()
	{
		return DiScenApiWrapper::GetInstance()->GetEntitiesCount();
	}

	int GetEntities(EntityData* entities, int* size)
	{
		return DiScenApiWrapper::GetInstance()->GetEntities(entities, size);
	}

	int GetElementsCount()
	{
		return DiScenApiWrapper::GetInstance()->GetEntitiesCount();
	}

	int GetElements(ElementData* elements, int* size)
	{
		return DiScenApiWrapper::GetInstance()->GetElements(elements, size);
	}

	Vector3f GetElementLocation(const char* id)
	{
		return DiScenApiWrapper::GetInstance()->GetElementLocation(id);
	}

	void AddElements(ElementData* elements, int size)
	{
		return DiScenApiWrapper::GetInstance()->AddElements(elements, size);
	}

	void AddElement(ElementData* elementData)
	{
		return DiScenApiWrapper::GetInstance()->AddElement(elementData);
	}

	Bool DeleteElement(const char* elementId)
	{
		return DiScenApiWrapper::GetInstance()->DeleteElement(elementId);
	}



	void ClearSimulation()
	{
		DiScenApiWrapper::GetInstance()->ClearSimulation();
	}

	Bool LoadSimulation(const char* path)
	{
		return DiScenApiWrapper::GetInstance()->LoadSimulation(path);
	}

	Bool SaveSimulation(const char* path)
	{
		return DiScenApiWrapper::GetInstance()->SaveSimulation(path);
	}

	void UpdateSimulation(float timeDelta)
	{
		DiScenApiWrapper::GetInstance()->UpdateSimulation(timeDelta);
	}

	Bool ValidSimulation()
	{
		return DiScenApiWrapper::GetInstance()->ValidSimulation();
	}

	Bool SimulationStarted()
	{
		return DiScenApiWrapper::GetInstance()->SimulationStarted();
	}

	void PlaySimulation()
	{
		DiScenApiWrapper::GetInstance()->PlaySimulation();
	}

	void PauseSimulation()
	{
		DiScenApiWrapper::GetInstance()->PauseSimulation();
	}

	void StopSimulation()
	{
		DiScenApiWrapper::GetInstance()->StopSimulation();
	}

	void SetSimulationProgress(double progress)
	{
		DiScenApiWrapper::GetInstance()->SetSimulationProgress(progress);
	}

	double ComputeSimulationProgress()
	{
		return DiScenApiWrapper::GetInstance()->ComputeSimulationProgress();
	}

	void RegisterSimulationLoadedCallback(EventCallback eventCallback)
	{
		DiScenApiWrapper::GetInstance()->RegisterSimulationLoadedCallback(eventCallback);
	}

	void RegisterSimulationUpdatedCallback(EventCallback eventCallback)
	{
		DiScenApiWrapper::GetInstance()->RegisterSimulationUpdatedCallback(eventCallback);
	}

	void RegisterSimulationPlayCallback(EventCallback eventCallback)
	{
		DiScenApiWrapper::GetInstance()->RegisterSimulationPlayCallback(eventCallback);
	}

	void RegisterSimulationPauseCallback(EventCallback eventCallback)
	{
		DiScenApiWrapper::GetInstance()->RegisterSimulationPauseCallback(eventCallback);
	}

	void RegisterSimulationStopCallback(EventCallback eventCallback)
	{
		DiScenApiWrapper::GetInstance()->RegisterSimulationStopCallback(eventCallback);
	}

	void RegisterSimulationTimeChangedCallback(EventFloatCallback eventCallback)
	{
		DiScenApiWrapper::GetInstance()->RegisterSimulationTimeChangedCallback(eventCallback);
	}

	const char* GetSimulationDateTimeAsString()
	{
		return DiScenApiWrapper::GetInstance()->GetSimulationDateTimeAsString();
	}

	void Deinitialize()
	{
		ClearScenario();
		//DiScenApiWrapper::DestroyInstance();
		//discenfw::LogMessage(DEBUG, "Deinitialized.", "DiScenFw");
	}
}




#endif

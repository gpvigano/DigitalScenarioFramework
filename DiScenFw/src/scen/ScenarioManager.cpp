//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/scen/ScenarioManager.h>
#include <discenfw/scen/ScenarioData.h>
#include <discenfw/scen/CommonData.h>

#include <discenfw/ve/VirtualEnvironmentAPI.h>
#include <discenfw/ve/VeManager.h>
#include <gpvulc/text/text_util.h>
#include <gpvulc/time/DateTimeUtil.h>
#include <discenfw/util/MessageLog.h>

#include <gpvulc/json/RapidJsonInclude.h> // ParseException, FormatException
#include "../JSON/JsonScenario.h"

#include <boost/filesystem.hpp>
#include <algorithm>
#include <sstream>

namespace discenfw
{
	using namespace ve;

	namespace scen
	{

		ScenarioManager::ScenarioManager()
		{
			InitScenario();
		}


		ScenarioManager::~ScenarioManager()
		{
		}


		void ScenarioManager::CreateSampleData()
		{
			Data = std::make_shared<Scenario>("TestData");
			AssetReference boxbpRepr = { "","","BoxBP",AssetReference::PROJECT };
			AssetReference conebpRepr = { "","","ConeBP",AssetReference::PROJECT };
			//std::vector<MutableElement> mutElemDefs = {
			//	{ "BoxBP1","BoxBP",{ 0, 250, 150 },{ 0, 0, 0 },{ 0.25, 0.25, 0.25 },{{boxbpRepr},{conebpRepr}},0 },
			//	{ "BoxBP2","BoxBP",{ 100, 250, 150 },{ 0, 0, 0 },{ 0.25, 0.25, 0.25 },{{boxbpRepr},{conebpRepr}},0 },
			//	{ "ConeBP1","ConeBP",{ -100, 250, 150 },{ 0, 0, 0 },{ 0.25, 0.25, 0.25 },{{conebpRepr},{boxbpRepr}},0 },
			//};
			//std::shared_ptr<MutableElement> mutElem;
			//for each (const MutableElement& mutElemdef in mutElemDefs)
			//{
			//	mutElem = std::make_shared<MutableElement>();
			//	*mutElem = mutElemdef;
			//	Data->Entities.push_back(mutElem);
			//}
			std::vector<Element> elemDefs = {
				{ "Box1","Test",{Vector3D{0, 100, 100},Vector3D{0, 0, 0},Vector3D{ 0.5, 0.5, 0.5 }},{ "","","Box",AssetReference::SCENE } },
				{ "Box2","Test",{Vector3D{100, 100, 100},Vector3D{0, 0, 0},Vector3D{ 0.5, 0.5, 0.5 }},{ "","","Box",AssetReference::SCENE } },
				{ "Box3","Test",{Vector3D{100, 100, 150},Vector3D{0, 0, 0},Vector3D{ 0.25, 0.25, 0.25 }},{boxbpRepr} },
				{ "Cone1","Test",{Vector3D{-100, 100, 100},Vector3D{0, 0, 0},Vector3D{ 0.5, 0.5, 0.5 }},{ "","","Cone",AssetReference::SCENE } },
				{ "Cone2","Test",{Vector3D{-100, 100, 150},Vector3D{0, 0, 0},Vector3D{ 0.25, 0.25, 0.25 }},{conebpRepr} },
			};
			std::shared_ptr<Element> elem;
			for (const Element& elemdef : elemDefs)
			{
				elem = std::make_shared<Element>();
				*elem = elemdef;
				Data->Entities.push_back(elem);
			}
			std::vector<ConnectionElement> pipeDefs = {
				{
					{"Pipeline1","ConnectionElement",{ "","ConnectionElement","ConnectionElement",AssetReference::SCENE }},
					{Vector3D{ -200, 200, 0 },Vector3D{ -200, 200, 200 },Vector3D{ 200, 200, 200 },Vector3D{ 200, 200, 0 }}
				},
				{
					{"Pipeline2","ConnectionElement",{ "","ConnectionElement","ConnectionElement",AssetReference::SCENE }},
					{Vector3D{ -200, 230, 0 },Vector3D{ -200, 230, 200 },Vector3D{ 200, 230, 200 },Vector3D{ 200, 230, 0 }}
				},
				//{"Pipeline2","ConnectionElement",{Vector3D{ -200, 330, 0 },Vector3D{ -200, 330, 200 },Vector3D{ 200, 330, 200 },Vector3D{ 200, 230, 0 }},{ "","ConnectionElement","PipelineBP",AssetReference::PROJECT }},
			};
			std::shared_ptr<Aggregate> pipeSys = std::make_shared<Aggregate>("PipeSystem1","PipeSystem");
			////std::shared_ptr<GroupElement> pipeSys = std::make_shared<GroupElement>();
			//pipeSys->Identity.Type = "PipeSystem";
			//pipeSys->Identity.Identifier = "PipeSystem1";
			Data->Entities.push_back(pipeSys);
			std::shared_ptr<ConnectionElement> pipe = nullptr;
			for (const ConnectionElement& pipedef : pipeDefs)
			{
				pipe = std::make_shared <ConnectionElement>();
				*pipe = pipedef;
				pipeSys->Components.push_back(pipe);
				////pipeSys->Parts.push_back(pipe);
			}

			std::vector<ConnectionElement> pipeDefs2 = {
				{
				{"Pipeline1bp","ConnectionElement",{ "","ConnectionElement","ConnectionElement",AssetReference::SCENE }},
				{Vector3D{ -200, 300, 0 },Vector3D{ -200, 300, 200 },Vector3D{ 200, 300, 200 },Vector3D{ 200, 300, 0 }},
				},
				{
				{"Pipeline2bp","ConnectionElement",{ "","ConnectionElement","PipelineBP",AssetReference::PROJECT }},
				{Vector3D{ -200, 330, 0 },Vector3D{ -200, 330, 200 },Vector3D{ 200, 330, 200 },Vector3D{ 200, 330, 0 }},
				},
			};
			AssetReference pipelinebpRepr = { "","","PipeSystemBP",AssetReference::PROJECT };
			std::shared_ptr<Aggregate> pipeSysBb = std::make_shared<Aggregate>("PipeSystem2bp", "PipeSystem", pipelinebpRepr);
			for (const ConnectionElement& pipedef : pipeDefs2)
			{
				pipe = std::make_shared <ConnectionElement>();
				*pipe = pipedef;
				pipeSysBb->Components.push_back(pipe);
				////pipeSys->Parts.push_back(pipe);
			}
			Data->Entities.push_back(pipeSysBb);
		}


		//void ScenarioManager::CreateDefaultLogic()
		//{
		//	//Logic->EntityCanBeSelected = [](const std::string& /*id*/) -> bool { return true; };
		//	//Logic->SetSelectedEntity = [&](const std::string& id) -> void { Logic->Interaction.ActiveEntity = GetEntityById(id); };
		//	//Logic->SetSelectedEntity = [&](const std::string& id) -> void { Logic->Selection.insert( GetEntityById(id) ); };
		//}


		bool ScenarioManager::ContainsEntity(const std::string& id) const
		{
			auto& entities = Data->Entities;
			const auto& entItr = std::find_if(entities.begin(), entities.end(),
				[&id](std::shared_ptr<Entity> entity) { return entity->GetIdentifier() ==id; });
			return (entItr != entities.end());
		}


		std::shared_ptr<Entity> ScenarioManager::GetEntityById(const std::string& id)
		{
			std::shared_ptr<Entity> entity = Data->GetEntityById(id);
			if (!entity)
			{
				LogMessage(LOG_ERROR, "Entity " + id + " not defined.", "DiScenFw", true, true, "NOT_AN_ENTITY");
			}

			return entity;
		}


		std::shared_ptr<Element> ScenarioManager::GetElementById(const std::string& id)
		{
			std::shared_ptr<Entity> entity = GetEntityById(id);
			if (!entity)
			{
				return nullptr;
			}
			if (entity->IsA("Element"))
			{
				std::shared_ptr<Element> elem = std::static_pointer_cast<Element>(entity);
				return elem;
			}
			LogMessage(LOG_ERROR, "Entity " + id + " is not an element.", "DiScenFw", true, true, "NOT_AN_ELEMENT");
			return nullptr;
		}


		void ScenarioManager::UpdateScenario(float timeDelta)
		{
			int apiLevel = VirtualEnvironmentAPI::LOGGING | VirtualEnvironmentAPI::SCENARIO;
			if (!VE()->Validated(apiLevel))
			{
				VE()->Validate(apiLevel);
			}
			ElapsedTime += timeDelta;
		}


		void ScenarioManager::ResetScenario()
		{
			InitScenario();
		}


		void ScenarioManager::InitScenario()
		{
			if (Data)
			{
				Data->Entities.clear();
				Data->Name.clear();
			}
			Data = std::make_shared<Scenario>("(New scenario)");
		}


		// only to read/write
		bool ScenarioManager::ScenarioFromJson(const std::string& jsonText)
		{
			//#ifdef _DEBUG
			//// (throwing exceptions)
			//scen::ScenarioFromJson(jsonText, *Data);
			//return true;
			//#else
			// (catching exceptions)
			std::string errMsg;
			try
			{
				scen::ScenarioFromJson(jsonText, *Data);
				return true;
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
			LogMessage(LOG_ERROR, "Error reading JSON:\n" + errMsg, "DiScenFw");
			return false;
			//#endif
		}


		void ScenarioManager::ScenarioToJson(std::string& jsonText)
		{
			scen::ScenarioToJson(*Data, jsonText);
		}


		bool ScenarioManager::LoadScenarioJson(const std::string& jsonPath, bool syncVe)
		{
			std::string jsonText;
			if (gpvulc::LoadText(jsonPath, jsonText) && ScenarioFromJson(jsonText))
			{
				ScenarioPath = jsonPath;
				if (syncVe)
				{
					if (VE()->SyncScene)
					{
						VE()->SyncScene();
					}
					else
					{
						LogMessage(LOG_WARNING, "SyncScene callback not defined.", "DiScenFw");
					}
				}
				LogMessage(LOG_DEBUG, "Scenario loaded from " + jsonPath, "DiScenFw");
				return true;
			}
			LogMessage(LOG_ERROR, "Failed to load " + jsonPath, "DiScenFw");
			return false;
		}


		bool ScenarioManager::SaveScenarioJson(const std::string& jsonPath, bool syncFromVe)
		{
			if (syncFromVe)
			{
				if (VE()->SyncScenario)
				{
					VE()->SyncScenario();
				}
				else
				{
					LogMessage(LOG_WARNING, "SyncScenario callback not defined.", "DiScenFw");
				}
			}
			std::string jsonText;
			ScenarioToJson(jsonText);
			if (gpvulc::SaveText(jsonPath, jsonText))
			{
				ScenarioPath = jsonPath;
				LogMessage(LOG_DEBUG, "Scenario saved to " + jsonPath, "DiScenFw");
				return true;
			}
			LogMessage(LOG_ERROR, "Failed to save " + jsonPath, "DiScenFw");

			return false;
		}

	} // namespace scen
}

#include "..\..\include\discenfw\sim\SimulationManager.h"
#include "..\..\include\discenfw\sim\SimulationManager.h"
//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/sim/SimulationManager.h>
#include <discenfw/sim/ScenarioHistoryData.h>
#include <discenfw/sim/HistoryLogParser.h>
#include "../JSON/JsonHistory.h"

#include <discenfw/scen/ScenarioData.h>
#include <discenfw/ve/VirtualEnvironmentAPI.h>
#include <discenfw/ve/VeManager.h>
#include <discenfw/util/MessageLog.h>

#include <gpvulc/text/text_util.h>
#include <gpvulc/time/DateTimeUtil.h>
#include <gpvulc/json/RapidJsonInclude.h> // ParseException, FormatException

#include <boost/filesystem.hpp>
#include <algorithm>
#include <sstream>

namespace discenfw
{
	using namespace ve;

	namespace sim
	{

		SimulationManager::SimulationManager()
		{
			Simulation = std::make_shared<SimulationController>();
			Simulation->Scenario = this->ScenarioData;
		}


		SimulationManager::~SimulationManager()
		{
		}



		void SimulationManager::SetScenarioData(std::shared_ptr<Scenario> scenario)
		{
			ScenarioData = scenario;
		}


		void SimulationManager::CreateSampleHistory()
		{
			//HistoryData = std::make_shared<ScenarioHistory>();
			HistoryData = Simulation->SimulationHistory;
			HistoryData->Uri = "Test.History";
			HistoryData->Name = "Test History";
			std::shared_ptr< EntityHistory > history = std::make_shared<EntityHistory>();
			//history->LogUri = "BOX1Anim.txt";
			{
				std::shared_ptr<ElementState> state = std::make_shared<ElementState>();
				state->StartDateTime = { 2020,07,15,9,0,0,0,0,0 };
				state->EndDateTime = { 2020,07,15,9,0,2,0,0,0 };
				state->Origin = "GeneratedSample";
				state->Transform.UseCoordSys = true;
				state->Transform.CoordSys.Origin = { 0,100.0f,0 };
				history->States.push_back(state);
			}
			{
				std::shared_ptr<ElementState> state = std::make_shared<ElementState>();
				state->StartDateTime = { 2020,07,15,9,0,5,0,0,0 };
				state->EndDateTime = { 2020,07,15,9,0,8,0,0,0 };
				state->Origin = "GeneratedSample";
				state->Transform.UseCoordSys = true;
				state->Transform.CoordSys.Origin = { 300.0f,100.0f,0 };
				history->States.push_back(state);
			}
			{
				std::shared_ptr<ElementState> state = std::make_shared<ElementState>();
				state->StartDateTime = { 2020,07,15,9,0,12,0,0,0 };
				state->EndDateTime = { 2020,07,15,9,0,15,0,0,0 };
				state->Origin = "GeneratedSample";
				state->Transform.UseCoordSys = true;
				state->Transform.CoordSys.RightAxis = { 1.0f,0,0 };
				state->Transform.CoordSys.ForwardAxis = { 0,0,-1.0f };
				state->Transform.CoordSys.UpAxis = { 0,1.0f,0 };
				state->Transform.CoordSys.Origin = { 0,0,0 };
				history->States.push_back(state);
			}
			//...
			HistoryData->EntityHistories["Box3"] = history;
			//LoadHistoryLogs();
			Simulation->Scenario = this->ScenarioData;
			Simulation->InitSimulation();
		}


		void SimulationManager::UpdateSimulation(float timeDelta)
		{
			ElapsedTime += timeDelta;

			if (Simulation->ValidSimulation())
			{
				if (!VE()->Validated(VirtualEnvironmentAPI::SIMULATION))
				{
					VE()->Validate(VirtualEnvironmentAPI::SIMULATION);
				}
				Simulation->Update(ElapsedTime);
			}
		}


		bool SimulationManager::HistoryFromJson(const std::string& jsonText)
		{
			if (!HistoryData)
			{
				HistoryData = std::make_shared<ScenarioHistory>();
				Simulation->SimulationHistory = HistoryData;
			}
			std::string errMsg = "unknown error";
			try
			{
				sim::HistoryFromJson(jsonText, *HistoryData);
				LoadHistoryLogs();
				Simulation->Scenario = this->ScenarioData;
				Simulation->InitSimulation();
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
		}


		void SimulationManager::HistoryToJson(std::string& jsonText)
		{
			sim::HistoryToJson(*HistoryData, jsonText);
		}


		bool SimulationManager::LoadHistoryJson(const std::string& jsonFile)
		{
			std::string jsonText;
			if (gpvulc::LoadText(jsonFile, jsonText) && HistoryFromJson(jsonText))
			{
				HistoryPath = jsonFile;
				return true;
			}
			LogMessage(LOG_ERROR, "Failed to load " + jsonFile, "DiScenFw|Sim");

			return false;
		}

		bool SimulationManager::SaveHistoryJson(const std::string& jsonFile)
		{
			std::string jsonText;
			HistoryToJson(jsonText);
			if (gpvulc::SaveText(jsonFile, jsonText))
			{
				HistoryPath = jsonFile;
				return true;
			}
			LogMessage(LOG_ERROR, "Failed to save " + jsonFile, "DiScenFw|Sim");

			return false;
		}

		void SimulationManager::ClearSimulation()
		{
			//if (Simulation) Simulation->StopSimulation();
			//Simulation = std::make_shared<SimulationController>();
			Simulation->SimulationHistory=std::make_shared<ScenarioHistory>();
			Simulation->Scenario = this->ScenarioData;
			HistoryData = Simulation->SimulationHistory;
		}


		void SimulationManager::LoadHistoryLogs()
		{
			boost::filesystem::path basePath(HistoryPath);
			basePath.remove_filename();
			for (const auto &pair : HistoryData->EntityHistories)
			{
				std::shared_ptr<EntityHistory> history = pair.second;
				if (!history->LogUri.empty() && history->States.empty())
				{
					HistoryLogParser logParser;
					boost::filesystem::path logUri(history->LogUri);
					boost::filesystem::path fullLogPath = basePath / logUri;
					std::string logPath = fullLogPath.string();
					if (!logParser.ParseFile(logPath, *history))
					{
						LogMessage(LOG_ERROR, "Failed to load " + logPath, "DiScenFw|Sim");
					}
				}
				//for (const auto& state : history->States)
				//{
				//	std::ostringstream oStream;
				//	oStream << "State duration: "
				//		<< gpvulc::DateTimeDistanceString(state->StartDateTime, state->EndDateTime)
				//		<< "="
				//		<< gpvulc::DateTimeDistanceSec(state->StartDateTime, state->EndDateTime)
				//		<< " sec.";
				//	LogMessage(LOG, oStream.str(), "DiScenFw|Sim");
				//}
			}
		}


	} // namespace sim
}

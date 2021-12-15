//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once


#include <DiScenFwConfig.h>
#include <discenfw/scen/ScenarioData.h>
#include "discenfw/sim/ScenarioHistoryData.h"
#include "discenfw/sim/SimulationController.h"

#include <string>
#include <memory>


namespace discenfw
{
	/*!
	Simulation management.
	*/
	namespace sim
	{
		/*!
		This manager holds simulation history data and provides methods to load, save and update it.
		*/
		class DISCENFW_API SimulationManager
		{
		// Prevent public access from DLL clients
		DISCENFW_DLL_PROTECTED

			/*!
			Default constructor: create a SimulationController.
			*/
			SimulationManager();

			~SimulationManager();

			/*!
			Shared pointer to Scenario data.
			*/
			std::shared_ptr<Scenario> ScenarioData;

			/*!
			Shared pointer to ScenarioHistory data.
			*/
			std::shared_ptr<ScenarioHistory> HistoryData;

			/*!
			Shared pointer to SimulationController.
			*/
			std::shared_ptr<SimulationController> Simulation;

		public:

			/*!
			Get the referenced scenario data.
			*/
			std::shared_ptr<Scenario> GetScenarioData()
			{
				return ScenarioData;
			}

			/*!
			Set the referenced scenario data.
			*/
			void SetScenarioData(std::shared_ptr<Scenario> scenario);

			/*!
			Get the owned SimulationController.
			*/
			std::shared_ptr<SimulationController> GetSimulationController()
			{
				return Simulation;
			}

			void CreateSampleHistory();

			/*!
			Parse a JSON text and fill history data.
			*/
			bool HistoryFromJson(const std::string& jsonText);

			/*!
			Write history data to a JSON text.
			*/
			void HistoryToJson(std::string& jsonText);

			/*!
			Load a JSON text file and fill history data.
			*/
			bool LoadHistoryJson(const std::string& jsonFile);

			/*!
			Write history data to a JSON text file.
			*/
			bool SaveHistoryJson(const std::string& jsonFile);


			/*!
			Update the simulation updating a VE (if connected).
			*/
			void UpdateSimulation(float timeDelta);

			/*!
			Clear history data.
			*/
			void ClearSimulation();

		private:

			double ElapsedTime = 0.0;
			std::string HistoryPath;
			std::string ErrorList;

			void LoadHistoryLogs();
		};
	}
}

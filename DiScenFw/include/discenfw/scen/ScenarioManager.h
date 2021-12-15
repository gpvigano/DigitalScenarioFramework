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
#include "discenfw/scen/ScenarioData.h"

#include <string>
#include <memory>


namespace discenfw
{
	/*!
	Scenario management.
	*/
	namespace scen
	{
		/*!
		This manager holds Scenario data and provides methods to load, save and update it.
		*/
		class DISCENFW_API ScenarioManager
		{
		// Prevent public access from DLL clients
		DISCENFW_DLL_PROTECTED

			/*!
			Default constructor: create an empty Scenario data.
			*/
			ScenarioManager();

			virtual ~ScenarioManager();

			std::shared_ptr<Scenario> GetData()
			{
				return Data;
			}

			int GetEntitiesCount()
			{
				return (int)Data->GetEntities().size();
			}


			/*!
			Create a sample scenario data (for debug only).
			*/
			void CreateSampleData();

			/*!
			Check if the scenario contains an entity with the given identifier.
			*/
			bool ContainsEntity(const std::string& id) const;

			/*!
			Get a shared pointer to an entity in the scenario given its identifier
			(or nullptr if not found).
			*/
			std::shared_ptr<Entity> GetEntityById(const std::string& id);

			/*!
			Get a shared pointer to an element in the scenario given its identifier
			(or nullptr if not found).
			*/
			std::shared_ptr<Element> GetElementById(const std::string& id);

			/*!
			Parse a JSON text and fill Scenario data.
			*/
			bool ScenarioFromJson(const std::string& jsonText);

			/*!
			Write Scenario data to a JSON text.
			*/
			void ScenarioToJson(std::string& jsonText);

			/*!
			Load a JSON text file and fill Scenario data.
			@param jsonPath path to the JSON text file
			@param syncVe call the synchronization function of the Virtual Environment
			*/
			bool LoadScenarioJson(const std::string& jsonPath, bool syncVe = false);

			/*!
			Write Scenario data to a JSON text file.
			*/
			bool SaveScenarioJson(const std::string& jsonPath, bool syncFromVe = false);

			/*!
			Update Scenario data (just checking API definition).
			*/
			virtual void UpdateScenario(float timeDelta);

			/*!
			Reset Scenario data.
			*/
			virtual void ResetScenario();

		private:

			/*!
			Shared pointer to Scenario data.
			*/
			std::shared_ptr<Scenario> Data;

			double ElapsedTime = 0.0;
			std::string ScenarioPath;

			void InitScenario();
		};
	}
}

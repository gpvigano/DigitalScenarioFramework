//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/scen/ScenarioData.h>

#include <string>


namespace discenfw
{
	namespace scen
	{

		/*!
		Parse a Scenario from a JSON text.
		*/
		void ScenarioFromJson(const std::string& jsonText, Scenario& scenarioData);


		/*!
		Serialize a Scenario to a JSON text.
		*/
		void ScenarioToJson(const Scenario& scenarioData, std::string& jsonText);

	}
}

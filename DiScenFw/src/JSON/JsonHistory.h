//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/sim/ScenarioHistoryData.h>

#include <string>


namespace discenfw
{
	namespace sim
	{
		/*!
		Parse a ScenarioHistory from a JSON text.
		*/
		void HistoryFromJson(const std::string& jsonText, ScenarioHistory& historyData);


		/*!
		Serialize a ScenarioHistory to a JSON text.
		*/
		void HistoryToJson(const ScenarioHistory& historyData, std::string& jsonText);
	}
}

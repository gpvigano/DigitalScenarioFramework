//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonScenarioParser.h"
#include "JsonScenarioWriter.h"

#include "JsonScenario.h"

using namespace discenfw;
using namespace discenfw::json;

namespace discenfw
{

	namespace scen
	{
		void ScenarioFromJson(const std::string& jsonText, Scenario& scenarioData)
		{
			JsonScenarioParser parser;
			parser.ParseScenario(jsonText, scenarioData);
			parser.CheckJsonErrors();
		}


		void ScenarioToJson(const Scenario& scenarioData, std::string& jsonText)
		{
			JsonScenarioWriter writer;
			writer.WriteScenario(scenarioData, jsonText);
		}
	}
}

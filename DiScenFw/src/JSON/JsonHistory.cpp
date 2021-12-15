//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonHistoryWriter.h"
#include "JsonHistoryParser.h"


//---------------------------------------------------------------------

#include "JsonHistory.h"

using namespace discenfw::json;

namespace discenfw
{
	namespace sim
	{
		void HistoryFromJson(const std::string& jsonText, ScenarioHistory& historyData)
		{
			JsonHistoryParser parser;
			parser.ParseScenarioHistory(jsonText, historyData);
			parser.CheckJsonErrors();
		}


		void HistoryToJson(const ScenarioHistory& historyData, std::string& jsonText)
		{
			JsonHistoryWriter writer;
			writer.WriteScenarioHistory(historyData, jsonText);
		}
	}
}

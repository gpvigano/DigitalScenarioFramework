//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonParserBase.h"
#include <discenfw/sim/ScenarioHistoryData.h>


namespace discenfw
{
	namespace json
	{
		class JsonHistoryParser : public JsonParserBase
		{
		public:

			JsonHistoryParser();

			void ParseScenarioHistory(const std::string& jsonText, sim::ScenarioHistory& historyData);

		protected:

			void ParseElementState(const rapidjson::Value& entityStateValue, std::shared_ptr<sim::ElementState>& elementState);
			std::shared_ptr<sim::TemporalState> ParseTemporalState(const rapidjson::Value& entityStateValue);
			std::shared_ptr<sim::EntityHistory> ParseEntityHistory(const rapidjson::Value& entityHistoryValue);
			std::shared_ptr<sim::HistoryEvent> ParseHistoryEvent(const rapidjson::Value& historyEventValue);
		};

	}
}


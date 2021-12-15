//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonWriterBase.h"
#include <discenfw/sim/ScenarioHistoryData.h>

#include <memory>

namespace discenfw
{
	namespace json
	{

		class JsonHistoryWriter : public JsonWriterBase
		{
		public:
			JsonHistoryWriter();

			void WriteScenarioHistory(const sim::ScenarioHistory& historyData, std::string& jsonText);
		protected:

			void WriteTemporalState(const std::shared_ptr<sim::TemporalState>& entityState);
			void WriteElementState(const std::shared_ptr<sim::ElementState>& elementState);
			void WriteHistory(const std::shared_ptr<sim::EntityHistory>& entityHistory);
			void WriteHistoryEvent(const std::shared_ptr<sim::HistoryEvent>& historyEvent);
		};

	}
}

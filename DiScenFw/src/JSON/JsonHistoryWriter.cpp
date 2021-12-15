//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonHistoryWriter.h"

#include <gpvulc/time/DateTimeUtil.h>


namespace discenfw
{
	namespace json
	{
		using namespace rapidjson;
		using namespace discenfw;
		using namespace discenfw::sim;


		JsonHistoryWriter::JsonHistoryWriter()
			: JsonWriterBase()
		{
		}


		void JsonHistoryWriter::WriteScenarioHistory(const sim::ScenarioHistory& historyData, std::string& jsonText)
		{
			StartDocument();
			StartObject("ScenarioHistory");
			SetDecimalPrecision(5);

			WriteString("Uri", historyData.Uri);
			WriteString("Name", historyData.Name);
			WriteString("Description", historyData.Description);
			WriteString("Details", historyData.Details);
			StartArray("EntityHistories");
			for (const auto &pair : historyData.EntityHistories)
			{
				StartObject();
				WriteString("Entity", pair.first);
				std::shared_ptr<EntityHistory> history = pair.second;
				WriteHistory(history);
				EndObject();
			}
			EndArray();

			StartArray("Events");
			for (unsigned i = 0; i < historyData.Events.size(); i++)
			{
				std::shared_ptr<HistoryEvent> historyEvent = historyData.Events[i];
				WriteHistoryEvent(historyEvent);
			}
			EndArray();

			EndObject();
			EndDocument(jsonText);
		}



		void JsonHistoryWriter::WriteTemporalState(const std::shared_ptr<TemporalState>& entityState)
		{
			WriteString("StartDateTime", gpvulc::DateTimeToString(entityState->StartDateTime));
			WriteString("EndDateTime", gpvulc::DateTimeToString(entityState->EndDateTime));
			WriteString("Origin", entityState->Origin);
		}


		void JsonHistoryWriter::WriteElementState(const std::shared_ptr<ElementState>& elementState)
		{
			WriteLocalTransform("LocalTransform", elementState->Transform);
			WriteString("Representation", elementState->Representation);
		}


		void JsonHistoryWriter::WriteHistory(const std::shared_ptr<EntityHistory>& entityHistory)
		{
			StartObject("History");
			if (!entityHistory->LogUri.empty())
			{
				WriteString("LogUri", entityHistory->LogUri);
			}
			//else
			{
				StartArray("States");
				for (unsigned i = 0; i < entityHistory->States.size(); i++)
				{
					std::shared_ptr<TemporalState> state = entityHistory->States[i];
					bool isElemState = state->IsA("ElementState");
					StartObject();
					WriteString("Class", state->GetClassName());
					WriteTemporalState(state);
					if (isElemState)
					{
						std::shared_ptr<ElementState> elemState = std::static_pointer_cast<ElementState>(state);
						WriteElementState(elemState);
					}
					EndObject();
				}
				EndArray();
			}
			EndObject();
		}


		void JsonHistoryWriter::WriteHistoryEvent(const std::shared_ptr<HistoryEvent>& historyEvent)
		{
			WriteString("Uri", historyEvent->Uri);
			WriteString("Category", historyEvent->Category);
			WriteString("DateTime", gpvulc::DateTimeToString(historyEvent->DateTime));
		}

	}
}

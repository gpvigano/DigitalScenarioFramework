//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonHistoryParser.h"

#include <gpvulc/time/DateTimeUtil.h>

using namespace rapidjson;
using namespace discenfw::sim;

namespace discenfw
{
	namespace json
	{

		JsonHistoryParser::JsonHistoryParser()
		{
		}

		void JsonHistoryParser::ParseScenarioHistory(const std::string& jsonText, sim::ScenarioHistory& historyData)
		{
			StartContext("ScenarioHistory");

			Parse(jsonText.c_str());

			const Value& scenario = GetRootElement("ScenarioHistory");
			historyData.Uri = GetAsString(scenario, "Uri");
			historyData.Name = GetAsString(scenario, "Name");
			historyData.Description = GetAsString(scenario, "Description", true);
			historyData.Details = GetAsString(scenario, "Details", true);
			if (CheckHasArray(scenario, "EntityHistories"))
			{
				StartContext("EntityHistories");
				const Value& histories = scenario["EntityHistories"];
				historyData.EntityHistories.clear();
				for (SizeType i = 0; i < histories.Size(); i++)
				{
					StartContext(std::to_string(i));
					historyData.EntityHistories[histories[i]["Entity"].GetString()] = ParseEntityHistory(histories[i]["History"]);
					EndContext();
				}
				EndContext();
			}
			if (CheckHasArray(scenario, "Events"))
			{
				StartContext("Events");
				const Value& events = scenario["Events"];
				historyData.Events.clear();
				for (SizeType i = 0; i < events.Size(); i++)
				{
					StartContext(std::to_string(i));
					historyData.Events.push_back(ParseHistoryEvent(events[i]));
					EndContext();
				}
				EndContext();
			}
			EndContext();
		}



		void JsonHistoryParser::ParseElementState(const rapidjson::Value& entityStateValue, std::shared_ptr<ElementState>& elementState)
		{
			const Value& transformValue = entityStateValue["LocalTransform"];
			StartContext("LocalTransform");
			ParseLocalTransform(transformValue, elementState->Transform);
			EndContext();
			elementState->Representation = GetAsString(entityStateValue, "Representation", true);
			/*if (element->IsMutableElementState())
			{
				std::shared_ptr <MutableElement> mutableElementState = std::static_pointer_cast<MutableElementState>(element);
				entityStateData->Representation = entityStateValue["Representation"].GetString();
				JsonHistoryParser::ParseMutableElementState(entityStateValue, mutableElementState);
			}*/
		}


		std::shared_ptr<TemporalState> JsonHistoryParser::ParseTemporalState(const rapidjson::Value& entityStateValue)
		{
			std::string stateClass(GetAsString(entityStateValue, "Class"));
			bool isElemState = (stateClass == "ElementState");

			std::shared_ptr<TemporalState> entityStateData = MakeEntityState(stateClass);
			entityStateData->StartDateTime = gpvulc::StringToDateTime(GetAsString(entityStateValue, "StartDateTime"));
			entityStateData->EndDateTime = gpvulc::StringToDateTime(GetAsString(entityStateValue, "EndDateTime"));
			entityStateData->Origin = GetAsString(entityStateValue, "Origin");
			if (isElemState)
			{
				std::shared_ptr <ElementState> elementState = std::static_pointer_cast<ElementState>(entityStateData);
				JsonHistoryParser::ParseElementState(entityStateValue, elementState);
			}

			return entityStateData;
		}


		std::shared_ptr<EntityHistory> JsonHistoryParser::ParseEntityHistory(const rapidjson::Value& entityHistoryValue)
		{
			std::shared_ptr<EntityHistory> entityHistory = std::make_shared<EntityHistory>();
			if (entityHistoryValue.HasMember("LogUri"))
			{
				entityHistory->LogUri = GetAsString(entityHistoryValue, "LogUri");
			}
			else
			{
				if (CheckHasArray(entityHistoryValue, "States"))
				{
					StartContext("States");
					const Value& statesValue = entityHistoryValue["States"];
					for (SizeType i = 0; i < statesValue.Size(); i++)
					{
						StartContext(i);
						entityHistory->States.push_back(JsonHistoryParser::ParseTemporalState(statesValue[i]));
						EndContext();
					}
					EndContext();
				}
			}
			return entityHistory;
		}


		std::shared_ptr<HistoryEvent> JsonHistoryParser::ParseHistoryEvent(const rapidjson::Value& historyEventValue)
		{
			std::shared_ptr<HistoryEvent> historyEventData = std::make_shared<HistoryEvent>();
			historyEventData->Uri = GetAsString(historyEventValue, "Uri");
			historyEventData->Category = GetAsString(historyEventValue, "Category");
			historyEventData->DateTime = gpvulc::StringToDateTime(GetAsString(historyEventValue, "DateTime"));
			return historyEventData;
		}

	}
}

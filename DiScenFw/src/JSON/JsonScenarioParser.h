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
#include <discenfw/scen/ScenarioData.h>


namespace discenfw
{
	namespace json
	{
		class JsonScenarioParser : public JsonParserBase
		{
		public:

			JsonScenarioParser();

			void ParseScenario(const std::string& jsonText, Scenario& scenarioData);

		protected:

			void ParseAggregate(const rapidjson::Value& entityValue, std::shared_ptr<Aggregate>& aggregate);
			void ParseGroupElement(const rapidjson::Value& entityValue, std::shared_ptr<GroupElement>& groupElement);
			void ParseSocketInfo(const rapidjson::Value& socketInfoValue, SocketInfo& socketInfo);
			Anchor ParseAnchor(const rapidjson::Value& anchorValue);
			void ParseConnection(const rapidjson::Value& entityValue, std::shared_ptr <ConnectionElement>& connection);
			void ParseMutableElement(const rapidjson::Value& entityValue, std::shared_ptr <MutableElement>& mutableElement);
			void ParseElement(const rapidjson::Value& entityValue, std::shared_ptr<Element>& element);
			EntityIdentity ParseEntityIdentity(const rapidjson::Value& entityIdentityValue);
			std::shared_ptr<Entity> ParseEntity(const rapidjson::Value& entityValue);
		};

	}
}


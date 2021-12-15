//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonRoleInfoParser.h"
#include <discenfw/xp/EnvironmentModel.h>


namespace discenfw
{
	namespace json
	{
		class JsonEnvironmentModelParser : public JsonRoleInfoParser
		{
		public:

			JsonEnvironmentModelParser();

			std::shared_ptr<xp::EnvironmentModel> ParseEnvironmentModel(const std::string& jsonText, const std::string& knowlJsonText = "");
			std::shared_ptr<xp::EnvironmentModel> ParseEnvironmentModelDefinition(const std::string& jsonText);
			std::shared_ptr<xp::EnvironmentModel> ParseEnvironmentModelKnowledge(const std::string& jsonText, std::shared_ptr<xp::EnvironmentModel>);

		protected:

			std::shared_ptr<xp::EntityStateType> ParseEntityStateType(const rapidjson::Value& entityStateTypeValue);
			std::shared_ptr<xp::EntityState> ParseEntityState(const rapidjson::Value& stateValue);
			std::shared_ptr<xp::EnvironmentState> ParseEnvironmentState(const rapidjson::Value& stateValue);
		};

	}
}


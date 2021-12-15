//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonCommonParser.h"
#include <discenfw/xp/RoleInfo.h>
#include <discenfw/xp/PropertyCondition.h>
#include <discenfw/xp/FeatureCondition.h>
#include <discenfw/xp/RelationshipCondition.h>
#include <discenfw/xp/EntityCondition.h>


namespace discenfw
{
	namespace json
	{
		class JsonRoleInfoParser : public JsonCommonParser
		{
		public:

			JsonRoleInfoParser();

			std::shared_ptr<xp::RoleInfo> ParseRoleInfo(const std::string& jsonText);

		protected:

			std::shared_ptr<xp::RoleInfo> ParseRoleDefinition(
				const rapidjson::Value& roleInfoValue,
				const char* requiredModelName = nullptr
				);

			xp::PropertyCondition ParsePropertyCondition(const rapidjson::Value& conditionValue);
			xp::FeatureCondition ParseFeatureCondition(const rapidjson::Value& conditionValue);
			xp::RelationshipCondition ParseRelationshipCondition(const rapidjson::Value& conditionValue);
			xp::EntityCondition ParseEntityCondition(const rapidjson::Value& conditionValue);
			xp::Condition ParseCondition(const rapidjson::Value& conditionValue);
			void ParseRewardRules(
				const rapidjson::Value& stateRewardValue,
				xp::StateRewardRules& stateReward
				);
		};

	}
}


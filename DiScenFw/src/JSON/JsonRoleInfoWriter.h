//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonCommonWriter.h"
#include <discenfw/xp/RoleInfo.h>
#include <discenfw/xp/PropertyCondition.h>
#include <discenfw/xp/FeatureCondition.h>
#include <discenfw/xp/RelationshipCondition.h>
#include <discenfw/xp/EntityCondition.h>

#include <string>

namespace discenfw
{
	namespace json
	{

		class JsonRoleInfoWriter : public JsonCommonWriter
		{
		public:
			JsonRoleInfoWriter();

			void WriteRoleInfo(const xp::RoleInfo& role, std::string& jsonText);
		protected:

			void WriteRole(const char* memberName, const xp::RoleInfo& role);
			void WriteRoleDefinition(const char* memberName, const xp::RoleInfo& role);
			void WritePropertyCondition(const char* memberName, const xp::PropertyCondition& condition);
			void WriteFeatureCondition(const char* memberName, const xp::FeatureCondition& condition);
			void WriteRelationshipCondition(const xp::RelationshipCondition& condition);
			void WriteEntityCondition(const char* memberName, const xp::EntityCondition& condition);
			void WriteCondition(const char* memberName, const xp::Condition& condition);
			void WriteRewardRules(const char* memberName, const xp::StateRewardRules& stateReward);
		};

	}
}

//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonRoleInfoWriter.h"


namespace discenfw
{
	namespace json
	{
		using namespace discenfw::xp;


		JsonRoleInfoWriter::JsonRoleInfoWriter()
		{
		}

		void JsonRoleInfoWriter::WriteRoleInfo(const RoleInfo& role, std::string& jsonText)
		{
			StartDocument();
			WriteRole("RoleInfo", role);
			EndDocument(jsonText);
		}


		void JsonRoleInfoWriter::WriteRole(const char* memberName, const xp::RoleInfo& role)
		{
			StartObject(memberName);

			WriteString("ModelName", role.ModelName);
			WriteString("RoleName", role.RoleName);
			WriteCondition("SuccessCondition", role.SuccessCondition);
			WriteCondition("FailureCondition", role.FailureCondition);
			WriteCondition("DeadlockCondition", role.DeadlockCondition);
			WriteRewardRules("StateReward", role.StateReward);

			EndObject();
		}


		void JsonRoleInfoWriter::WriteRoleDefinition(const char* memberName, const xp::RoleInfo & role)
		{
			StartObject(memberName);

			WriteString("RoleName", role.RoleName);
			WriteString("ModelName", role.ModelName);
			WriteCondition("SuccessCondition", role.SuccessCondition);
			WriteCondition("FailureCondition", role.FailureCondition);
			WriteCondition("DeadlockCondition", role.DeadlockCondition);
			WriteRewardRules("StateReward", role.StateReward);

			EndObject();
		}


		void JsonRoleInfoWriter::WritePropertyCondition(const char* memberName, const PropertyCondition& condition)
		{
			StartObject(memberName);
			WriteString("Name", condition.PropertyName);
			WriteString("Operator", CompOpToString(condition.ComparisonOperator));
			if (condition.ComparisonOperator != CompOp::DEFINED)
			{
				WriteString("Value", condition.PropertyValue);
			}
			EndObject();
		}


		void JsonRoleInfoWriter::WriteFeatureCondition(const char* memberName, const xp::FeatureCondition & condition)
		{
			StartArray(memberName);
			WriteString(nullptr, condition.FeatureName);
			WriteString(nullptr, CompOpToString(condition.ComparisonOperator));
			if (condition.ComparisonOperator != CompOp::DEFINED)
			{
				WriteString(nullptr, condition.FeatureValue);
			}
			EndArray();
		}


		void JsonRoleInfoWriter::WriteRelationshipCondition(const RelationshipCondition& condition)
		{
			StartObject();
			WriteString("LinkId", condition.RelatingLinkId);
			WriteString("RelatedEntityId", condition.Related.EntityId);
			WriteString("RelatedLinkId", condition.Related.LinkId);
			if (condition.Unrelated)
			{
				WriteBool("Unrelated", condition.Unrelated);
			}
			EndObject();
		}


		void JsonRoleInfoWriter::WriteEntityCondition(const char* memberName, const EntityCondition& condition)
		{
			StartObject(memberName);

			WriteString("EntityId", condition.EntityId);
			if (!condition.TypeName.empty())
			{
				WriteString("TypeName", condition.TypeName);
			}

			StartArray("PropConditions");
			for (const auto& item : condition.PropConditions)
			{
				WritePropertyCondition(nullptr, item);
			}
			EndArray();

			StartArray("RelConditions");
			for (const auto& item : condition.RelConditions)
			{
				WriteRelationshipCondition(item);
			}
			EndArray();

			EndObject();
		}


		void JsonRoleInfoWriter::WriteCondition(const char* memberName, const Condition& condition)
		{
			StartObject(memberName);

			StartArray("EntityConditions");
			for (const auto& item : condition.EntityConditions)
			{
				WriteEntityCondition(nullptr, item);
			}
			EndArray();

			StartArray("FeatureConditions");
			for (const auto& item : condition.FeatureConditions)
			{
				WriteFeatureCondition(nullptr, item);
			}
			EndArray();

			StartArray("RelatedConditions");
			for (int i = 0; i < (int)condition.RelatedConditions.size(); i++)
			{
				StartObject();
				WriteString("LogicOperator", LogicOpToString(condition.RelatedConditions[i].first));
				WriteCondition("Condition", *(condition.RelatedConditions[i].second));
				EndObject();
			}
			EndArray();

			EndObject();
		}


		void JsonRoleInfoWriter::WriteRewardRules(const char* memberName, const StateRewardRules& stateReward)
		{
			auto valueOrDefault = [](const std::map< ActionResult, int >& reward, ActionResult which)
			{
				if (reward.find(which) == reward.cend()) return 0;
				return reward.at(which);
			};

			StartObject(memberName);
			StartObject("ResultReward");
			WriteInt("IN_PROGRESS", valueOrDefault(stateReward.ResultReward, ActionResult::IN_PROGRESS));
			WriteInt("SUCCEEDED", valueOrDefault(stateReward.ResultReward, ActionResult::SUCCEEDED));
			WriteInt("FAILED", valueOrDefault(stateReward.ResultReward, ActionResult::FAILED));
			WriteInt("DEADLOCK", valueOrDefault(stateReward.ResultReward, ActionResult::DEADLOCK));
			EndObject();

			if (!stateReward.CumulativeRewards.empty())
			{
				StartArray("CumulativeRewards");
				for (int i = 0; i < (int)stateReward.CumulativeRewards.size(); i++)
				{
					StartObject();
					WriteString("TypeName", stateReward.CumulativeRewards[i].TypeName);
					WritePropertyCondition("Filter", stateReward.CumulativeRewards[i].Filter);
					WriteInt("Reward", stateReward.CumulativeRewards[i].Reward);
					EndObject();
				}
				EndArray();
			}

			if (!stateReward.EntityConditionRewards.empty())
			{
				StartArray("EntityConditionRewards");
				for (const auto& item : stateReward.EntityConditionRewards)
				{
					StartObject();
					WriteEntityCondition("EntityCondition", item.first);
					WriteInt("Reward", item.second);
					EndObject();
				}
				EndArray();
			}

			if (!stateReward.FeatureRewards.empty())
			{
				StartArray("FeatureRewards");
				for (const auto& item : stateReward.FeatureRewards)
				{
					StartObject();
					WriteFeatureCondition("FeatureCondition", item.first);
					WriteInt("Reward", item.second);
					EndObject();
				}
				EndArray();
			}
			EndObject();
		}

	}
}

//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonRoleInfoParser.h"


namespace discenfw
{
	namespace json
	{
		using namespace rapidjson;
		using namespace discenfw::xp;

		JsonRoleInfoParser::JsonRoleInfoParser()
		{
		}


		std::shared_ptr<xp::RoleInfo> JsonRoleInfoParser::ParseRoleInfo(const std::string& jsonText)
		{
			StartContext("RoleInfo");

			Parse(jsonText);

			const Value& roleInfoValue = GetRootElement("RoleInfo");
			std::shared_ptr<RoleInfo> roleInfo = ParseRoleDefinition(roleInfoValue);

			EndContext();
			xp::GetModel(roleInfo->ModelName)->StoreRole(roleInfo);
			return roleInfo;
		}


		std::shared_ptr<xp::RoleInfo> JsonRoleInfoParser::ParseRoleDefinition(
			const rapidjson::Value& roleInfoValue,
			const char* requiredModelName
			)
		{
			std::string modelName = GetAsString(roleInfoValue, "ModelName");

			if (requiredModelName != nullptr && modelName != requiredModelName)
			{
				AddJsonError(gpvulc::json::ErrorType::INVALID_VALUE, "ModelName (different)");
				return nullptr;
			}
			std::string roleName = GetAsString(roleInfoValue, "RoleName");
			std::shared_ptr<RoleInfo> roleInfo = std::make_shared<RoleInfo>(roleName, modelName);
			if (CheckHasMembers(roleInfoValue, { "SuccessCondition","FailureCondition","DeadlockCondition" }))
			{
				StartContext("SuccessCondition");
				roleInfo->SuccessCondition = ParseCondition(roleInfoValue["SuccessCondition"]);
				EndContext();
				StartContext("FailureCondition");
				roleInfo->FailureCondition = ParseCondition(roleInfoValue["FailureCondition"]);
				EndContext();
				StartContext("DeadlockCondition");
				roleInfo->DeadlockCondition = ParseCondition(roleInfoValue["DeadlockCondition"]);
				EndContext();
			}
			if (CheckHasMember(roleInfoValue, "StateReward"))
			{
				StartContext("StateReward");
				ParseRewardRules(roleInfoValue["StateReward"], roleInfo->StateReward);
				EndContext();
			}

			return roleInfo;
		}


		PropertyCondition JsonRoleInfoParser::ParsePropertyCondition(const rapidjson::Value& conditionValue)
		{
			PropertyCondition cond;
			cond.PropertyName = GetAsString(conditionValue, "Name");
			std::string op = GetAsString(conditionValue, "Operator");
			cond.ComparisonOperator = CompOpFromString(op);
			if (cond.ComparisonOperator != CompOp::DEFINED)
			{
				cond.PropertyValue = GetAsString(conditionValue, "Value");
			}
			return cond;
		}

		FeatureCondition JsonRoleInfoParser::ParseFeatureCondition(const rapidjson::Value& featuresValue)
		{
			FeatureCondition cond;
			if (CheckIsArray(featuresValue))
			{
				cond.FeatureName = GetString(featuresValue[0], "feature name");
				std::string op = GetString(featuresValue[1], "Operator");
				cond.ComparisonOperator = CompOpFromString(op);
				if (cond.ComparisonOperator != CompOp::DEFINED)
				{
					cond.FeatureValue = GetString(featuresValue[2], "feature");
				}
			}
			return cond;
		}


		RelationshipCondition JsonRoleInfoParser::ParseRelationshipCondition(const rapidjson::Value& conditionValue)
		{
			RelationshipCondition cond;
			cond.RelatingLinkId = GetAsString(conditionValue, "LinkId");
			cond.Related.EntityId = GetAsString(conditionValue, "RelatedEntityId");
			cond.Related.LinkId = GetAsString(conditionValue, "RelatedLinkId");
			cond.Unrelated = GetAsBool(conditionValue, "Unrelated", true, false);
			return cond;
		}


		EntityCondition JsonRoleInfoParser::ParseEntityCondition(const rapidjson::Value& conditionValue)
		{
			EntityCondition condition;
			condition.EntityId = GetAsString(conditionValue, "EntityId");
			condition.TypeName = GetAsString(conditionValue, "TypeName", true);
			if (CheckHasArray(conditionValue, "PropConditions", true))
			{
				const Value& propCondValue = conditionValue["PropConditions"];
				StartContext("PropConditions");
				for (SizeType i = 0; i < propCondValue.Size(); i++)
				{
					StartContext(i);
					condition.PropConditions.push_back(ParsePropertyCondition(propCondValue[i]));
					EndContext();
				}
				EndContext();
			}
			if (CheckHasArray(conditionValue, "RelConditions", true))
			{
				const Value& relCondValue = conditionValue["RelConditions"];
				StartContext("RelConditions");
				for (SizeType i = 0; i < relCondValue.Size(); i++)
				{
					StartContext(i);
					condition.RelConditions.push_back(ParseRelationshipCondition(relCondValue[i]));
					EndContext();
				}
				EndContext();
			}
			return condition;
		}


		Condition JsonRoleInfoParser::ParseCondition(const rapidjson::Value& conditionValue)
		{
			Condition condition;

			if (CheckHasArray(conditionValue, "EntityConditions", true))
			{
				const Value& entCondValue = conditionValue["EntityConditions"];
				StartContext("EntityConditions");
				for (SizeType i = 0; i < entCondValue.Size(); i++)
				{
					StartContext(i);
					condition.EntityConditions.push_back(ParseEntityCondition(entCondValue[i]));
					EndContext();
				}
				EndContext();
			}

			if (CheckHasArray(conditionValue, "FeatureConditions", true))
			{
				const Value& featCondValue = conditionValue["FeatureConditions"];
				StartContext("FeatureConditions");
				for (SizeType i = 0; i < featCondValue.Size(); i++)
				{
					StartContext(i);
					condition.FeatureConditions.push_back(ParseFeatureCondition(featCondValue[i]));
					EndContext();
				}
				EndContext();
			}

			if (CheckHasArray(conditionValue, "RelatedConditions", true))
			{
				StartContext("RelatedConditions");
				const Value& relCondValue = conditionValue["RelatedConditions"];
				for (SizeType i = 0; i < relCondValue.Size(); i++)
				{
					StartContext(i);
					LogicOp op = LogicOpFromString(GetAsString(relCondValue[i], "LogicOperator"));
					if (CheckHasMember(relCondValue[i], "Condition"))
					{
						const Value& propCondValue = relCondValue[i]["Condition"];
						StartContext("Condition");
						condition.RelatedConditions.push_back({ op, std::make_shared<Condition>(ParseCondition(propCondValue)) });
						EndContext();
					}
					EndContext();
				}
				EndContext();
			}
			return condition;
		}

		void JsonRoleInfoParser::ParseRewardRules(const rapidjson::Value& stateRewardValue, StateRewardRules& stateReward)
		{
			if (CheckHasMember(stateRewardValue, "ResultReward"))
			{
				const Value& resultRewardValue = stateRewardValue["ResultReward"];
				stateReward.ResultReward[ActionResult::IN_PROGRESS] = GetAsInt(resultRewardValue, "IN_PROGRESS");
				stateReward.ResultReward[ActionResult::SUCCEEDED] = GetAsInt(resultRewardValue, "SUCCEEDED");
				stateReward.ResultReward[ActionResult::FAILED] = GetAsInt(resultRewardValue, "FAILED");
				stateReward.ResultReward[ActionResult::DEADLOCK] = GetAsInt(resultRewardValue, "DEADLOCK");
			}

			stateReward.CumulativeRewards.clear();
			if (CheckHasArray(stateRewardValue, "CumulativeRewards", true))
			{
				StartContext("CumulativeRewards");
				const Value& crValue = stateRewardValue["CumulativeRewards"];
				for (SizeType i = 0; i < crValue.Size(); i++)
				{
					StartContext(i);
					if (CheckHasMember(crValue[i], "Filter"))
					{
						StartContext("Filter");
						PropertyCondition propCond = ParsePropertyCondition(crValue[i]["Filter"]);
						EndContext();
						stateReward.CumulativeRewards.push_back({
							GetAsString(crValue[i],"TypeName"),
							propCond,
							GetAsInt(crValue[i],"Reward")
						});
					}
					EndContext();
				}
				EndContext();
			}

			stateReward.EntityConditionRewards.clear();
			if (CheckHasArray(stateRewardValue, "EntityConditionRewards", true))
			{
				StartContext("EntityConditionRewards");
				const Value& propRewardValue = stateRewardValue["EntityConditionRewards"];
				for (SizeType i = 0; i < propRewardValue.Size(); i++)
				{
					StartContext(i);
					if (CheckHasMember(propRewardValue[i], "EntityCondition"))
					{
						StartContext("EntityCondition");
						EntityCondition entityCond = ParseEntityCondition(propRewardValue[i]["EntityCondition"]);
						EndContext();
						stateReward.EntityConditionRewards.push_back({ entityCond, GetAsInt(propRewardValue[i],"Reward") });
					}
					EndContext();
				}
				EndContext();
			}

			stateReward.FeatureRewards.clear();
			if (CheckHasArray(stateRewardValue, "FeatureRewards", true))
			{
				StartContext("FeatureRewards");
				const Value& featRewardValue = stateRewardValue["FeatureRewards"];
				for (SizeType i = 0; i < featRewardValue.Size(); i++)
				{
					StartContext(i);
					if (CheckHasArray(featRewardValue[i], "FeatureCondition"))
					{
						StartContext("FeatureCondition");
						FeatureCondition featureCond = ParseFeatureCondition(featRewardValue[i]["FeatureCondition"]);
						EndContext();
						stateReward.FeatureRewards.push_back({ featureCond, GetAsInt(featRewardValue[i],"Reward") });
					}
					EndContext();
				}
				EndContext();
			}
		}


	}
}

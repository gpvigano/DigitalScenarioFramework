//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/Condition.h>

namespace discenfw
{
	namespace xp
	{
		Condition::Condition()
		{
		}


		Condition::Condition(
			const std::vector<EntityCondition>& entityCondition)
			: EntityConditions(entityCondition)
		{
		}


		Condition::Condition(
			const std::vector<FeatureCondition>& featureConditions)
			: EntityConditions(), FeatureConditions(featureConditions)
		{
		}


		bool Condition::Evaluate(const std::shared_ptr<EnvironmentState> environmentState) const
		{
			bool result = Evaluate(*environmentState);
			return result;
		}


		bool Condition::Evaluate(const EnvironmentState& environmentState) const
		{
			bool result = false;

			// evaluate this condition

			if (!EntityConditions.empty() || !FeatureConditions.empty())
			{
				result = true;
				if (!EntityConditions.empty())
				{
					for (const auto& entityCond : EntityConditions)
					{
						bool eval = entityCond.Evaluate(environmentState);
						if (!eval)
						{
							result = false;
							break;
						}
					}
				}

				if (result && !FeatureConditions.empty())
				{
					for (const auto& featureCond : FeatureConditions)
					{
						bool eval = featureCond.Evaluate(environmentState);
						if (!eval)
						{
							result = false;
							break;
						}
					}
				}
			}

			// evaluate related conditions...
			if (!RelatedConditions.empty())
			{
				for (const auto& relatedCondPair : RelatedConditions)
				{
					LogicOp conditionOperator = relatedCondPair.first;
					const auto& relatedCond = relatedCondPair.second;
					bool relatedResult = false;
					switch (conditionOperator)
					{
					case LogicOp::AND:
						result = result && relatedCond->Evaluate(environmentState);
						if (!result)
						{
							return false;
						}
						break;
					case LogicOp::OR:
						result = result || relatedCond->Evaluate(environmentState);
						break;
					case LogicOp::AND_NOT:
						result = result && !relatedCond->Evaluate(environmentState);
						if (!result)
						{
							return false;
						}
						break;
					case LogicOp::OR_NOT:
						result = result || !relatedCond->Evaluate(environmentState);
						break;
					case LogicOp::XOR:
						relatedResult = relatedCond->Evaluate(environmentState);
						result = (result && !relatedResult) || (!result && relatedResult);
						break;
					default:
						break;
					}
				}
			}

			return result;
		}


		void Condition::SetEntityCondition(const EntityCondition& entityCondition)
		{
			const std::string& entityId = entityCondition.EntityId;
			auto itr = std::find_if(EntityConditions.begin(), EntityConditions.end(),
				[&entityId](EntityCondition& cond)
			{
				return cond.EntityId == entityId;
			});
			if (itr != EntityConditions.end())
			{
				*itr = entityCondition;
			}
			else
			{
				EntityConditions.push_back(entityCondition);
			}
		}


		void Condition::SetFeatureCondition(const FeatureCondition& featureCondition)
		{
			const std::string& featName = featureCondition.FeatureName;
			auto itr = std::find_if(FeatureConditions.begin(), FeatureConditions.end(),
				[&featName](FeatureCondition& cond)
			{
				return cond.FeatureName == featName;
			});
			if (itr != FeatureConditions.end())
			{
				*itr = featureCondition;
			}
			else
			{
				FeatureConditions.push_back(featureCondition);
			}
		}


		void Condition::AddCondition(
			LogicOp logicOperator,
			const Condition& relatedCondition)
		{
			RelatedConditions.push_back({ logicOperator,relatedCondition.Clone() });
		}


		std::shared_ptr<Condition> Condition::Clone() const
		{
			std::shared_ptr<Condition> condition = std::make_shared<Condition>();
			*condition = (const Condition&)(*this);
			return condition;
		}



		bool Condition::operator ==(const Condition& condition) const
		{
			if (RelatedConditions.size() != condition.RelatedConditions.size())
			{
				return false;
			}
			for (size_t i = 0; i < RelatedConditions.size(); i++)
			{
				if (RelatedConditions[i].first != condition.RelatedConditions[i].first
					|| !RelatedConditions[i].second->EqualTo(condition.RelatedConditions[i].second))
				{
					return false;
				}
			}
			return EntityConditions == condition.EntityConditions && FeatureConditions == condition.FeatureConditions;
		}


		Condition& Condition::operator =(const Condition& condition)
		{
			if (this == &condition)
			{
				return *this;
			}
			RelatedConditions.clear();
			for (const auto& cond : condition.RelatedConditions)
			{
				RelatedConditions.push_back({ cond.first,cond.second->Clone() });
			}
			EntityConditions = condition.EntityConditions;
			FeatureConditions = condition.FeatureConditions;
			return *this;
		}

	} // namespace xp
} // namespace discenfw

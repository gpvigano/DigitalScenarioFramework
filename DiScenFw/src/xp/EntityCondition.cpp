//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/xp/EntityCondition.h>

namespace discenfw
{
	namespace xp
	{

		const std::string EntityCondition::ANY = "___ANY_ENTITY___";
		const std::string EntityCondition::ALL = "___ALL_ENTITIES___";


		bool EntityCondition::EvaluatePropConditions(const EntityState& entityState) const
		{
			for (const auto& propCond : PropConditions)
			{
				bool eval = propCond.Evaluate(entityState);
				if (!eval)
				{
					return false;
				}
			}
			return true;
		}


		bool EntityCondition::EvaluateRelConditions(const EntityState& entityState) const
		{
			for (const auto& relCond : RelConditions)
			{
				bool related = false;
				for (const auto& relationship : entityState.Relationships)
				{
					bool eval = relCond.Evaluate(relationship.first, relationship.second);
					if (eval)
					{
						related = true;
						break;
					}
				}
				if (!related)
				{
					return false;
				}
			}
			return true;
		}


		bool EntityCondition::Evaluate(const std::shared_ptr<EnvironmentState> environmentState) const
		{
			bool result = Evaluate(*environmentState);
			return result;
		}


		bool EntityCondition::Evaluate(const EnvironmentState& environmentState) const
		{
			if (!Defined())
			{
				return false;
			}

			// evaluate this condition
			bool allEntitiesTagFound = (EntityId == EntityCondition::ALL);
			bool anyEntityTagFound = (EntityId == EntityCondition::ANY);
			if (allEntitiesTagFound || anyEntityTagFound)
			{
				for (const auto& entityStateItr : environmentState.EntityStates)
				{
					if (TypeName.empty()
						|| entityStateItr.second->IsOfType(TypeName))
					{
						if (!PropConditions.empty())
						{
							const std::shared_ptr<EntityState> entityState = entityStateItr.second;
							bool propEval = PropConditions.empty() || EvaluatePropConditions(*entityState);
							bool eval = propEval;
							if (eval && !RelConditions.empty())
							{
								eval = EvaluateRelConditions(*entityState);
							}
							if (allEntitiesTagFound && !eval)
							{
								return false;
							}
							if (anyEntityTagFound && eval)
							{
								return true;
							}
						}
					}
				}
				if (anyEntityTagFound)
				{
					return false;
				}
				if (allEntitiesTagFound)
				{
					return true;
				}
			}
			else
			{
				const auto& entityStateItr = environmentState.EntityStates.find(EntityId);
				bool entityStateFound = entityStateItr != environmentState.EntityStates.cend();
				if (!entityStateFound)
				{
					return false;
				}
				std::shared_ptr<EntityState> entityState = entityStateItr->second;
				if (TypeName.empty()
					|| entityState->IsOfType(TypeName))
				{
					if (!PropConditions.empty())
					{
						bool eval = EvaluatePropConditions(*entityState);

						if (!eval)
						{
							return false;
						}
					}
					if (!RelConditions.empty())
					{
						bool eval = EvaluateRelConditions(*entityState);

						if (!eval)
						{
							return false;
						}
					}
				}
			}

			return true;
		}

	} // namespace xp
} // namespace discenfw

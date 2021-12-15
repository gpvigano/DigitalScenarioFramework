//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <DiScenFwConfig.h>
#include "discenfw/xp/EnvironmentState.h"
#include "discenfw/xp/PropertyCondition.h"
#include "discenfw/xp/RelationshipCondition.h"

#include <string>



namespace discenfw
{
	namespace xp
	{

		/*!
		Data structure for property condition evaluation.
		*/
		class EntityCondition
		{
		public:
			static DISCENFW_API const std::string ANY; //!< Special tag for any entity.
			static DISCENFW_API const std::string ALL; //!< Special tag for all the entities.

			/*!
			Identifier of the entity to which the property to be evaluated belongs.
			*/
			std::string EntityId;

			/*!
			Name of the entity state type needed for this entity (ignored if empty).
			*/
			std::string TypeName;

			/*!
			Property conditions to be evaluated.
			*/
			std::vector<PropertyCondition> PropConditions;

			/*!
			Relationship conditions to be evaluated.
			*/
			std::vector<RelationshipCondition> RelConditions;

			/*!
			Construct an empty (undefined) condition.
			*/
			EntityCondition()
			{
			}


			/*!
			Construct a property condition given the property name, its value and the comparison operator.
			*/
			EntityCondition(
			const std::string& entityId,
				const std::vector<PropertyCondition>& propertyConditions,
				const std::vector<RelationshipCondition>& relationshipConditions = {},
				const std::string& typeName = "")
				: EntityId(entityId),
				TypeName(typeName),
				PropConditions(propertyConditions),
				RelConditions(relationshipConditions)
			{
			}


			bool operator ==(const EntityCondition& condition) const;

			/*!
			Check if this condion is defined.
			*/
			bool Defined() const;

			/*!
			Evaluate this condition against the given environment state.
			*/
			bool Evaluate(const std::shared_ptr<EnvironmentState> environmentState) const;

			/*!
			Evaluate this condition against the given environment state.
			*/
			bool Evaluate(const EnvironmentState& environmentState) const;

		protected:

			/*!
			Evaluate only property conditions against the given entity state.
			*/
			bool EvaluatePropConditions(const EntityState& entityState) const;

			/*!
			Evaluate only relationship conditions against the given entity state.
			*/
			bool EvaluateRelConditions(const EntityState& entityState) const;
		};


		inline bool EntityCondition::Defined() const
		{
			return !EntityId.empty() && (
				!TypeName.empty()
				|| !PropConditions.empty()
				|| !RelConditions.empty()
				);
		}


		inline bool EntityCondition::operator ==(const EntityCondition& condition) const
		{
			return
				EntityId == condition.EntityId &&
				TypeName == condition.TypeName &&
				PropConditions == condition.PropConditions &&
				RelConditions == condition.RelConditions;
		}

	}

}


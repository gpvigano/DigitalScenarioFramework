//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/xp/EntityState.h"
#include <discenfw/util/CompOp.h>
#include <string>



namespace discenfw
{
	namespace xp
	{

		/*!
		Data structure for property condition evaluation.
		*/
		class PropertyCondition
		{
		public:
			/*!
			Name/identifier of the property to be evaluated.
			*/
			std::string PropertyName;

			/*!
			Reference value to be evaluated.
			*/
			std::string PropertyValue;

			/*!
			Comparison operator (between the reference value and the actual value).
			*/
			CompOp ComparisonOperator = CompOp::EQUAL;

			PropertyCondition()
				: PropertyName(), PropertyValue()
			{
			}

			/*!
			Construct a property condition given the property name and its value (equality comparison).
			*/
			PropertyCondition(const std::string& propertyName, const std::string& propertyValue)
				: PropertyName(propertyName), PropertyValue(propertyValue)
			{
			}

			/*!
			Construct a property condition given the property name, its value and the comparison operator.
			*/
			PropertyCondition(const std::string& propertyName, CompOp comparisonOperator, const std::string& propertyValue)
				: PropertyName(propertyName), PropertyValue(propertyValue), ComparisonOperator(comparisonOperator)
			{
			}

			bool operator ==(const PropertyCondition& condition) const;

			PropertyCondition& operator =(const PropertyCondition& condition);

			/*!
			Evaluate this condition against the given entity state.
			*/
			bool Evaluate(const EntityState& entityState) const;
		};


		inline bool PropertyCondition::operator ==(const PropertyCondition& condition) const
		{
			return
				PropertyName == condition.PropertyName &&
				PropertyValue == condition.PropertyValue &&
				ComparisonOperator == condition.ComparisonOperator;
		}


		inline PropertyCondition& PropertyCondition::operator =(const PropertyCondition& condition)
		{
			PropertyName = condition.PropertyName;
			PropertyValue = condition.PropertyValue;
			ComparisonOperator = condition.ComparisonOperator;
			return *this;
		}
	}

}


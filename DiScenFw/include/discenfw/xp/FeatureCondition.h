//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/xp/EnvironmentState.h"
#include <discenfw/util/CompOp.h>
#include <string>



namespace discenfw
{
	namespace xp
	{

		/*!
		Data structure for feature condition evaluation.
		*/
		class FeatureCondition
		{
		public:
			/*!
			Name/identifier of the feature to be evaluated.
			*/
			std::string FeatureName;

			/*!
			Reference value to be evaluated.
			*/
			std::string FeatureValue;

			/*!
			Comparison operator (between the reference value and the actual value).
			*/
			CompOp ComparisonOperator = CompOp::EQUAL;

			FeatureCondition()
				: FeatureName(), FeatureValue()
			{
			}

			/*!
			Construct a feature condition given the feature name and its value (equality comparison).
			*/
			FeatureCondition(const std::string& featureName, const std::string& featureValue)
				: FeatureName(featureName), FeatureValue(featureValue)
			{
			}

			/*!
			Construct a feature condition given the feature name, its value and the comparison operator.
			*/
			FeatureCondition(const std::string& featureName, CompOp comparisonOperator, const std::string& featureValue)
				: FeatureName(featureName), FeatureValue(featureValue), ComparisonOperator(comparisonOperator)
			{
			}

			/*!
			Construct a feature condition given the feature name, its value and the comparison operator as string.
			*/
			FeatureCondition(const std::string& featureName, const std::string& comparisonOpName, const std::string& featureValue)
				: FeatureName(featureName), FeatureValue(featureValue), ComparisonOperator(CompOpFromString(comparisonOpName))
			{
			}

			bool operator ==(const FeatureCondition& condition) const;

			FeatureCondition& operator =(const FeatureCondition& condition);

			/*!
			Evaluate this condition against the given environment state.
			*/
			bool Evaluate(const EnvironmentState& state) const;
		};


		inline bool FeatureCondition::operator ==(const FeatureCondition& condition) const
		{
			return
				FeatureName == condition.FeatureName &&
				FeatureValue == condition.FeatureValue &&
				ComparisonOperator == condition.ComparisonOperator;
		}


		inline FeatureCondition& FeatureCondition::operator =(const FeatureCondition& condition)
		{
			FeatureName = condition.FeatureName;
			FeatureValue = condition.FeatureValue;
			ComparisonOperator = condition.ComparisonOperator;
			return *this;
		}
	}

}


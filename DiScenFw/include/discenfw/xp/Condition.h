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

#include "discenfw/xp/EntityCondition.h"
#include "discenfw/xp/FeatureCondition.h"
#include "discenfw/xp/EnvironmentState.h"

#include <discenfw/util/LogicOp.h>

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <algorithm>


namespace discenfw
{
	namespace xp
	{

		/*!
		Entity condition that can be logically connected to other conditions.
		*/
		struct DISCENFW_API Condition
		{
		// Prevent public access from DLL clients
		DISCENFW_DLL_PROTECTED

			/*!
			Entity conditions.
			*/
			std::vector<EntityCondition> EntityConditions;

			/*!
			Feature conditions.
			*/
			std::vector<FeatureCondition> FeatureConditions;


			/*!
			List of related conditions connected with logical operators.
			*/
			std::vector< std::pair< LogicOp, std::shared_ptr<Condition> > > RelatedConditions;

		public:

			/*!
			Construct an empty (undefined) condition.
			*/
			Condition();

			/*!
			Construct a condition from a list of entity conditions.
			*/
			Condition(const std::vector<EntityCondition>& entityConditions);

			/*!
			Construct a condition from a list of feature conditions.
			*/
			Condition(const std::vector<FeatureCondition>& featureConditions);

			/*!
			Get the list of entity conditions.
			*/
			const std::vector<EntityCondition>& GetEntityConditions() const
			{
				return EntityConditions;
			}

			/*!
			Get the list of related conditions connected with logical operators.
			*/
			const std::vector< std::pair< LogicOp, std::shared_ptr<Condition> > >& GetRelatedConditions() const
			{
				return RelatedConditions;
			}

			/*!
			Set a condition for the given entity.
			*/
			void SetEntityCondition(const EntityCondition& entityCondition);

			/*!
			Set a condition for the given feature.
			*/
			void SetFeatureCondition(const FeatureCondition& featureCondition);


			/*!
			Add a related condition, joint with the given logic operator.
			*/
			void AddCondition(
				LogicOp logicOperator,
				const Condition& relatedCondition);


			/*!
			Evaluate the condition in the given environment state.
			*/
			bool Evaluate(const std::shared_ptr<EnvironmentState> environmentState) const;


			/*!
			Evaluate the condition in the given environment state.
			*/
			bool Evaluate(const EnvironmentState& environmentState) const;


			/*!
			Compare the given referenced condition to this one.
			*/
			bool EqualTo(const std::shared_ptr<Condition>& condition) const;


			/*!
			Clone this instance and return a shared pointer to it.
			*/
			std::shared_ptr<Condition> Clone() const;


			/*!
			Check if this condion (or a related condition) is defined.
			*/
			bool Defined() const;


			bool operator ==(const Condition& condition) const;


			Condition& operator =(const Condition& condition);

		};



		inline bool Condition::EqualTo(const std::shared_ptr<Condition>& condition) const
		{
			return *this == *condition;
		}


		inline bool Condition::Defined() const
		{
			return !EntityConditions.empty() || !FeatureConditions.empty() || !RelatedConditions.empty();
		}

	}
}

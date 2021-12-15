//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/xp/PropertyCondition.h"

#include <string>

namespace discenfw
{
	namespace xp
	{
		/*!
		Reward for each entity of a given type, with the given condition satisfied.
		*/
		struct PropertyReward
		{
			/*!
			Type of entity state to be considered.
			*/
			std::string TypeName;

			/*!
			Condition to be satisfied
			*/
			PropertyCondition Filter;

			/*!
			Reward assigned for each matching entity.
			*/
			int Reward = 0;

			///@name Constructors
			///@{

			PropertyReward()
			{
			}

			PropertyReward(std::string typeName, PropertyCondition filter, int reward) :
				TypeName(typeName), Filter(filter), Reward(reward)
			{
			}

			///@}
		};

	}
}


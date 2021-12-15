//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/xp/PropertyReward.h"
#include "discenfw/xp/PropertyCondition.h"
#include "discenfw/xp/FeatureCondition.h"
#include "discenfw/xp/ActionResult.h"

#include <string>
#include <map>
#include <vector>

namespace discenfw
{
	namespace xp
	{
		/*!
		Reward computed on a scenario state according to a set of conditions.
		*/
		struct StateRewardRules
		{
			/*!
			Reward given by the episode result.
			@remark If the episode is a succeess and the IN_PROGRESS value is defined,
			that value affects the episode performance calculation according to the mode
			set by the Experience. If the Experience::DiscountingConstant is set to
			a negative value it is computed as the ratio between IN_PROGRESS value and
			the SUCCEEDED value in this map.
			@see Experience
			*/
			std::map< ActionResult, int > ResultReward
			{
				{ ActionResult::IN_PROGRESS,-1 },
				{ ActionResult::SUCCEEDED,10 },
				{ ActionResult::FAILED,-10 },
				{ ActionResult::DEADLOCK,-5 },
			};

			/*!
			Optional rewards for each entity matching some property conditions (for any entity of the given type).
			*/
			std::vector< PropertyReward > CumulativeRewards;

			/*!
			Optional rewards related to the given entity conditions.
			*/
			std::vector< std::pair<EntityCondition, int> > EntityConditionRewards;

			/*!
			Optional rewards related to the given features.

			@remark If the comparison operator is CompOp::DEFINED the reward
			is multipied by the actual feature value.
			*/
			std::vector< std::pair<FeatureCondition, int> > FeatureRewards;

			StateRewardRules()
			{
			}

			StateRewardRules(
				const std::map< ActionResult, int >& resultReward,
				const std::vector< PropertyReward >& cumulativeRewards,
				const std::vector< std::pair<EntityCondition, int> >& entityConditionRewards,
				const std::vector< std::pair<FeatureCondition, int> >& featureRewards
				) :
				ResultReward(resultReward),
				CumulativeRewards(cumulativeRewards),
				EntityConditionRewards(entityConditionRewards),
				FeatureRewards(featureRewards)
			{
			}

		};

	}
}


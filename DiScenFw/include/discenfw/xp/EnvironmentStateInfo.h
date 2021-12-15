//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/xp/ActionResult.h"

namespace discenfw
{
	namespace xp
	{
		/*!
		Environment state information observed by an agent.
		*/
		struct EnvironmentStateInfo
		{
			/*!
			Computed reward of this scenario state.
			*/
			int Reward = 0;

			/*!
			Result
			*/
			ActionResult Result = ActionResult::IN_PROGRESS;


			/*!
			Check if the episode was successfully completed.
			*/
			bool Succeded() { return Result == ActionResult::SUCCEEDED; }

			/*!
			Check if the episode was a failure.
			*/
			bool Failed() { return Result == ActionResult::FAILED; }

			/*!
			Check if the episode was completed.
			*/
			bool IsCompleted() { return Result != ActionResult::IN_PROGRESS; }

			/*!
			Check if the episode was started but not completed.
			*/
			bool IsInProgress() { return Result == ActionResult::IN_PROGRESS; }

			/*!
			True if this state is identified as a terminal state.
			*/
			bool IsTerminal()
			{
				return Result == ActionResult::SUCCEEDED || Result == ActionResult::FAILED;
			}

			EnvironmentStateInfo() {}

			/*!
			Construct a scenario state based on the given entity states.
			*/
			EnvironmentStateInfo(int reward, ActionResult result)
				: Reward(reward), Result(result)
			{
			}
		};

	}
}


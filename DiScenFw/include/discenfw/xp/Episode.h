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
#include "discenfw/xp/Action.h"
#include "discenfw/xp/Transition.h"

#include <vector>


namespace discenfw
{
	namespace xp
	{
		/*!
		Data structure holding the action sequence and initial and final state of an episode.
		*/
		struct Episode
		{
			std::shared_ptr<EnvironmentState> InitialState; //!< Initial state for the episode.
			std::vector<Transition> TransitionSequence; //!< Sequence of actions done with related information.
			std::shared_ptr<EnvironmentState> LastState; //!< Last state for the episode.
			int Performance = 0; //!< Episode performance measure, used to choose the best episode.
			ActionResult Result = ActionResult::IN_PROGRESS; //!< Episode result.
			int RepetitionsCount = 0; //!< Number of times this episode was repeated.


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
			bool Completed() { return Result != ActionResult::IN_PROGRESS; }

			/*!
			Check if the episode was started but not completed.
			*/
			bool InProgress() { return Result == ActionResult::IN_PROGRESS; }

			/*!
			Check if the episode was started but not completed.
			*/
			bool Empty() { return TransitionSequence.empty(); }
		};

	}
}


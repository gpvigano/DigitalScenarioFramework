//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/xp/ref.h"

#include <memory>


namespace discenfw
{
	namespace xp
	{
		/*!
		Transition between states.
		*/
		struct Transition
		{
			/*!
			State from which the transition starts.
			*/
			std::shared_ptr<EnvironmentState> StartState;

			/*!
			Action taken that causes the transition.
			*/
			std::shared_ptr<Action> ActionTaken;

			/*!
			State to which the transition ends.
			*/
			std::shared_ptr<EnvironmentState> EndState;


			Transition()
			{
			}


			Transition(
				std::shared_ptr<EnvironmentState> StartState,
				std::shared_ptr<Action> actionTaken,
				std::shared_ptr<EnvironmentState> EndState
				) :
				StartState(StartState),
				ActionTaken(actionTaken),
				EndState(EndState)
			{
			}


			bool operator ==(const Transition& transition) const
			{
				return StartState == transition.StartState
					&& ActionTaken == transition.ActionTaken
					&& EndState == transition.EndState;
			}


			bool operator <(const Transition& transition) const
			{
				// Create an order based on pointers, just for maps.

				if (StartState < transition.StartState) return true;
				if (StartState > transition.StartState) return false;
				// ==> StartState == transition.StartState
				if (ActionTaken < transition.ActionTaken) return true;
				if (ActionTaken > transition.ActionTaken) return false;
				// ==> ActionTaken == transition.ActionTaken
				return EndState < transition.EndState;
			}

		};
	}
}


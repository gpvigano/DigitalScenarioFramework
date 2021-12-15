//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <memory>


namespace discenfw
{
	namespace xp
	{
		// forward declarations

		class EnvironmentState;
		struct Action;

		/*!
		Alias for shared pointers to EnvironmentState.
		*/
		using StateRef = std::shared_ptr<EnvironmentState>;


		/*!
		Alias for shared pointers to Action.
		*/
		using ActionRef = std::shared_ptr<Action>;

		/*!
		Structure to hold references to a state and an action taken from that state.
		*/
		struct StateActionRef
		{
			StateRef State;
			ActionRef Action;

			StateActionRef()
			{
			}


			StateActionRef(
				StateRef stateRef,
				ActionRef actionRef
				) :
				State(stateRef),
				Action(actionRef)
			{
			}

			bool operator ==(const StateActionRef& stateAction) const
			{
				return State == stateAction.State
					&& Action == stateAction.Action;
			}


			bool operator <(const StateActionRef& stateAction) const
			{
				// Create an order based on pointers, just for maps.

				if (State < stateAction.State) return true;
				if (State > stateAction.State) return false;
				// ==> State == stateAction.State
				return (Action < stateAction.Action);
			}
		};
	}
}


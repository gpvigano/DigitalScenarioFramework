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

#include <memory>
#include <vector>
#include <string>


namespace discenfw
{
	namespace xp
	{

		/*!
		Result of an action.
		*/
		enum class ActionResult
		{
			IN_PROGRESS = 0, //!< Task still in progress
			FAILED, //!< Failure, something went wrong
			SUCCEEDED, //!< Success, task completed
			DEADLOCK, //!< Deadlock, task came to a deadlock
			DENIED, //!< Action denied, unable to take the action
		};


		inline std::string ActionResultToString(ActionResult actionResult)
		{
			switch (actionResult)
			{
			case ActionResult::FAILED:
				return "Failed";
			case ActionResult::SUCCEEDED:
				return "Succeeded";
			case ActionResult::DEADLOCK:
				return "Deadlock";
			case ActionResult::DENIED:
				return "Denied";
			case ActionResult::IN_PROGRESS:
			default:
				return "";
				break;
			}
		}


		inline ActionResult ActionResultFromString(const std::string& actionResult)
		{
			if (actionResult == "Succeeded") return ActionResult::SUCCEEDED;
			if (actionResult == "Failed") return ActionResult::FAILED;
			if (actionResult == "Deadlock") return ActionResult::DEADLOCK;
			if (actionResult == "Denied") return ActionResult::DENIED;
			return ActionResult::IN_PROGRESS;
		}

	}
}


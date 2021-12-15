//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

namespace discenfw
{
	namespace xp
	{
		/*!
		Agent statistics.
		*/
		struct AgentStats
		{
			int FailedCount = 0;
			int SuccessCount = 0;
			int DeadlockCount = 0;
			int ActionChoiceCount = 0;
			int ExplorationActionCount = 0;
			int StatesCount = 0;
			int EpisodesCount = 0;
		};
	}
}


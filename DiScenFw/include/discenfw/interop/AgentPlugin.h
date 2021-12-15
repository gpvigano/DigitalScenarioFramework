//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/RL/IAgent.h"

#include <discenfw/xp/Transition.h>

#include <boost/config.hpp>

namespace discenfw
{
	namespace xp
	{
		/*!
		Definition of a generic agent implementation.
		All agent implementations (plugins) must derive from this abstract class.
		*/
		class BOOST_SYMBOL_VISIBLE AgentPlugin : public IAgent
		{
		public:

			virtual ~AgentPlugin() {}
		};
	}
}

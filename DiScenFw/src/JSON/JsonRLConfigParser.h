//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonParserBase.h"
#include <discenfw/RL/RLConfig.h>


namespace discenfw
{
	namespace json
	{
		class JsonRLConfigParser : public JsonParserBase
		{
		public:

			JsonRLConfigParser();

			xp::RLConfig ParseRLConfig(const std::string& jsonText);
		};

	}
}


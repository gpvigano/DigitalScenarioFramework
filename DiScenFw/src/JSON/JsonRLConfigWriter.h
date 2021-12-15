//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonWriterBase.h"
#include <discenfw/RL/RLConfig.h>

#include <memory>

namespace discenfw
{
	namespace json
	{

		class JsonRLConfigWriter : public JsonWriterBase
		{
		public:
			JsonRLConfigWriter();

			void WriteRLConfig(const xp::RLConfig& rlConfig, std::string& jsonText);
		};

	}
}

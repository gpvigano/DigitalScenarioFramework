//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/RL/RLConfig.h>

#include <string>
#include <memory>


namespace discenfw
{
	namespace xp
	{

		/*!
		Parse a RL configuration from a JSON text.
		*/
		void RLConfigFromJson(const std::string& jsonText, RLConfig& rlConfig);


		/*!
		Serialize a RL configuration to a JSON text.
		*/
		void RLConfigToJson(const RLConfig& rlConfig, std::string& jsonText);

	}
}

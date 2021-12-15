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
#include <discenfw/xp/Experience.h>
#include <discenfw/xp/EnvironmentModel.h>


namespace discenfw
{
	namespace json
	{
		class JsonCommonParser : public JsonParserBase
		{
		public:

			JsonCommonParser();

		protected:

			xp::Action ParseAction(const rapidjson::Value& actionValue);

			void ParseTransition(
				std::shared_ptr<xp::EnvironmentModel> model,
				const rapidjson::Value& transitionValue,
				xp::Transition& transition
				);

			void ParseStateActionValue(
				std::shared_ptr<xp::EnvironmentModel> model,
				const rapidjson::Value& stateActionValue,
				xp::StateActionRef& stateAction,
				float& actionValue
				);
		};

	}
}


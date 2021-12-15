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
#include <discenfw/xp/Action.h>
#include <discenfw/xp/Experience.h>
#include <discenfw/xp/EnvironmentModel.h>

#include <memory>

namespace discenfw
{
	namespace json
	{

		class JsonCommonWriter : public JsonWriterBase
		{
		public:
			JsonCommonWriter();

		protected:

			void WriteAction(const char* memberName, const xp::Action& action);

			void WriteTransition(
				std::shared_ptr<xp::EnvironmentModel> model,
				const xp::Transition& transition,
				const char* name = nullptr
				);

			void WriteStateActionValue(
				std::shared_ptr<xp::EnvironmentModel> model,
				const xp::StateActionRef& stateAction,
				float value,
				const char* name = nullptr
				);
		};

	}
}

//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonCommonWriter.h"
#include <discenfw/xp/Experience.h>
#include <discenfw/xp/EnvironmentModel.h>

#include <memory>

namespace discenfw
{
	namespace json
	{

		class JsonExperienceWriter : public JsonCommonWriter
		{
		public:
			JsonExperienceWriter();

			void WriteExperience(const std::shared_ptr<xp::Experience> experience, std::string& jsonText);
		protected:

			void WriteEntityState(const char* memberName, const std::shared_ptr<const xp::EntityState> entState);
			void WriteEnvironmentState(int stateIndex, const std::shared_ptr<const xp::EnvironmentState> state);
			void WriteEpisode(const std::shared_ptr<xp::EnvironmentModel> model, const std::shared_ptr<const xp::Episode> episode);
		};

	}
}

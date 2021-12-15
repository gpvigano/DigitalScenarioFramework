//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonCommonParser.h"
#include <discenfw/xp/Experience.h>


namespace discenfw
{
	namespace json
	{
		class JsonExperienceParser : public JsonCommonParser
		{
		public:

			JsonExperienceParser();

			std::shared_ptr<xp::Experience> ParseExperienceAttributes(const std::string& jsonText);
			std::shared_ptr<xp::Experience> ParseExperience(const std::string& jsonText, std::shared_ptr<xp::Experience> experience = nullptr);

		protected:

			std::shared_ptr<xp::Experience> ParseExperienceDefinition(const std::string& jsonText, std::shared_ptr<xp::Experience> experience);
			xp::Action ParseAction(const rapidjson::Value& actionValue);
			std::shared_ptr<xp::Episode> ParseEpisode(const xp::Experience& experience, const rapidjson::Value& episodeValue);
		};

	}
}


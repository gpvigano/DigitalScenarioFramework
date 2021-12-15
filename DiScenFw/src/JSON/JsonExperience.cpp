//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonExperienceParser.h"
#include "JsonExperienceWriter.h"
#include "JsonExperience.h"

using namespace discenfw::json;
using namespace discenfw::xp;


namespace discenfw
{
	namespace xp
	{
		std::shared_ptr<Experience> ExperienceAttributesFromJson(const std::string& jsonText)
		{
			JsonExperienceParser parser;
			std::shared_ptr<Experience> experience = parser.ParseExperienceAttributes(jsonText);
			parser.CheckJsonErrors();
			return experience;
		}


		std::shared_ptr<Experience> ExperienceFromJson(const std::string& jsonText, std::shared_ptr<xp::Experience> experience)
		{
			JsonExperienceParser parser;
			experience = parser.ParseExperience(jsonText, experience);
			parser.CheckJsonErrors();
			return experience;
		}


		void ExperienceToJson(const std::shared_ptr<Experience>& experience, std::string& jsonText)
		{
			JsonExperienceWriter writer;
			writer.WriteExperience(experience, jsonText);
		}
	}
}


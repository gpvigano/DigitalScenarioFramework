//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/xp/Experience.h>

#include <string>
#include <memory>


namespace discenfw
{
	namespace xp
	{

		/*!
		Parse the basic attributes of an Experience from a JSON text.
		*/
		std::shared_ptr<Experience> ExperienceAttributesFromJson(const std::string& jsonText);

		/*!
		Parse an Experience from a JSON text.
		*/
		std::shared_ptr<Experience> ExperienceFromJson(const std::string& jsonText, std::shared_ptr<xp::Experience> experience = nullptr);


		/*!
		Serialize an Experience to a JSON text.
		*/
		void ExperienceToJson(const std::shared_ptr<Experience>& experience, std::string& jsonText);

	}
}

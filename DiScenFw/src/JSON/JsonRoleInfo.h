//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/xp/RoleInfo.h>

#include <string>
#include <memory>


namespace discenfw
{
	namespace xp
	{

		/*!
		Parse an RoleInfo from a JSON text.
		*/
		std::shared_ptr<RoleInfo> RoleInfoFromJson(const std::string& jsonText);


		/*!
		Serialize an RoleInfo to a JSON text.
		*/
		void RoleInfoToJson(const RoleInfo& roleInfo, std::string& jsonText);

	}
}

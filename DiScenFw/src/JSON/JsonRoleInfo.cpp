//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonRoleInfoParser.h"
#include "JsonRoleInfoWriter.h"

using namespace discenfw::json;
using namespace discenfw::xp;


namespace discenfw
{
	namespace xp
	{
		std::shared_ptr<RoleInfo> RoleInfoFromJson(const std::string& jsonText)
		{
			JsonRoleInfoParser parser;
			std::shared_ptr<RoleInfo> roleInfo = parser.ParseRoleInfo(jsonText);
			parser.CheckJsonErrors();
			return roleInfo;
		}

		void RoleInfoToJson(const RoleInfo& roleInfo, std::string& jsonText)
		{
			JsonRoleInfoWriter writer;
			writer.WriteRoleInfo(roleInfo, jsonText);
		}
	}
}


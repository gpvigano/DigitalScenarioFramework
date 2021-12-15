//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonRLConfigParser.h"
#include "JsonRLConfigWriter.h"

using namespace discenfw::xp;
using namespace discenfw::json;


namespace discenfw
{
	namespace xp
	{

		void RLConfigFromJson(const std::string& jsonText, RLConfig& qlConfig)
		{
			JsonRLConfigParser parser;
			qlConfig = parser.ParseRLConfig(jsonText);
			parser.CheckJsonErrors();
		}

		void RLConfigToJson(const RLConfig& qlConfig, std::string& jsonText)
		{
			JsonRLConfigWriter writer;
			writer.WriteRLConfig(qlConfig, jsonText);
		}
	}
}


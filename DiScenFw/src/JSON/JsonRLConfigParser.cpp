//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonRLConfigParser.h"

namespace discenfw
{
	namespace json
	{
		JsonRLConfigParser::JsonRLConfigParser()
		{
		}


		xp::RLConfig JsonRLConfigParser::ParseRLConfig(const std::string& jsonText)
		{
			using namespace rapidjson;
			using namespace discenfw::xp;

			StartContext("RLConfig");
			RLConfig rlConfig;

			// Parse a JSON string into DOM.
			Document doc;
			doc.Parse(jsonText);
			const Value& qlConfigValue = doc["RLConfig"];

			rlConfig.InitialValue = GetAsFloat(qlConfigValue, "InitialValue", true, 0.0f);
			rlConfig.SampleAverage = GetAsBool(qlConfigValue, "SampleAverage", true, false);
			rlConfig.FixedStepSize = GetAsFloat(qlConfigValue, "FixedStepSize", true, rlConfig.SampleAverage);
			rlConfig.DiscountRate = GetAsFloat(qlConfigValue, "DiscountRate");
			rlConfig.Epsilon = GetAsFloat(qlConfigValue, "Epsilon");

			EndContext();
			return rlConfig;
		}
	}
}


//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonRLConfigWriter.h"


namespace discenfw
{
	namespace json
	{
		using namespace rapidjson;
		using namespace discenfw;

		JsonRLConfigWriter::JsonRLConfigWriter()
			: JsonWriterBase()
		{
		}


		void JsonRLConfigWriter::WriteRLConfig(const xp::RLConfig& rlConfig, std::string& jsonText)
		{
			StartDocument();
			StartObject("RLConfig");

			SetDecimalPrecision(5);

			if (rlConfig.InitialValue != 0.0f)
			{
				WriteFloat("InitialValue", rlConfig.InitialValue);
			}
			if (rlConfig.SampleAverage)
			{
				WriteBool("SampleAverage", rlConfig.SampleAverage);
			}
			else
			{
				WriteFloat("FixedStepSize", rlConfig.FixedStepSize);
			}
			WriteFloat("DiscountRate", rlConfig.DiscountRate);
			WriteFloat("Epsilon", rlConfig.Epsilon);

			EndObject();
			EndDocument(jsonText);
		}
	}
}


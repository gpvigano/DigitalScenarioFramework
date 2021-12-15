//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include "JsonEnvironmentModelParser.h"
#include "JsonEnvironmentModelWriter.h"
#include "JsonEnvironmentModel.h"

using namespace discenfw::json;
using namespace discenfw::xp;


namespace discenfw
{
	namespace xp
	{
		std::shared_ptr<EnvironmentModel> EnvironmentModelFromJson(
			const std::string& jsonText,
			const std::string& knowlJsonText
			)
		{
			JsonEnvironmentModelParser parser;
			std::shared_ptr<EnvironmentModel> environmentModel;
			environmentModel = parser.ParseEnvironmentModel(jsonText, knowlJsonText);
			parser.CheckJsonErrors();
			return environmentModel;
		}

		void EnvironmentModelToJson(
			const std::shared_ptr<EnvironmentModel>& environmentModel,
			std::string& jsonText,
			std::string& knowlJsonText
			)
		{
			JsonEnvironmentModelWriter writer;
			writer.WriteEnvironmentModel(environmentModel, jsonText, knowlJsonText);
		}

		void EnvironmentModelDefinitionToJson(
			const std::shared_ptr<EnvironmentModel>& environmentModel,
			std::string& jsonText
			)
		{
			JsonEnvironmentModelWriter writer;
			writer.WriteEnvironmentModelDefinition(environmentModel, jsonText);
		}
	}
}


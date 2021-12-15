//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "JsonRoleInfoWriter.h"
#include <discenfw/xp/EnvironmentModel.h>

#include <memory>

namespace discenfw
{
	namespace json
	{

		class JsonEnvironmentModelWriter : public JsonRoleInfoWriter
		{
		public:
			JsonEnvironmentModelWriter();

			void WriteEnvironmentModel(
				const std::shared_ptr<xp::EnvironmentModel> environmentModel,
				std::string& jsonText,
				std::string& knowlJsonText
				);
			void WriteEnvironmentModelDefinition(const std::shared_ptr<xp::EnvironmentModel> environmentModel, std::string& jsonText);
			void WriteEnvironmentModelKnowledge(const std::shared_ptr<xp::EnvironmentModel> environmentModel, std::string& jsonText);
			void WriteEntityStateTypes(const std::shared_ptr<xp::EnvironmentModel> environmentModel, std::string& jsonText);
		protected:

			void WriteEntityState(const char* memberName, const std::shared_ptr<const xp::EntityState> entState);
			void WriteEnvironmentState(int stateIndex, const std::shared_ptr<const xp::EnvironmentState> state);
			void WriteEntityStateType(const char* memberName, const std::shared_ptr<xp::EntityStateType> entityStateType);
		};

	}
}

//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <discenfw/xp/EnvironmentModel.h>

#include <string>
#include <memory>


namespace discenfw
{
	namespace xp
	{

		/*!
		Parse an EnvironmentModel from a JSON text.
		*/
		std::shared_ptr<EnvironmentModel> EnvironmentModelFromJson(
			const std::string& jsonText,
			const std::string& knowlJsonText = ""
		);


		/*!
		Serialize an EnvironmentModel to a JSON text.
		*/
		void EnvironmentModelToJson(
			const std::shared_ptr<EnvironmentModel>& environmentModel,
			std::string& jsonText, std::string& knowlJsonText
		);


		/*!
		Serialize the definition of an EnvironmentModel to a JSON text.
		*/
		void EnvironmentModelDefinitionToJson(
			const std::shared_ptr<EnvironmentModel>& environmentModel,
			std::string& jsonText
		);
	}
}

//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <DiScenFwConfig.h>
#include "discenfw/ve/VirtualEnvironmentAPI.h"
#include <memory>

namespace discenfw
{
	/*!
	Virtual Environment API management.
	*/
	namespace ve
	{
		/*!
		Global access to the VirtualEnvironmentAPI.
		*/
		class DISCENFW_API VeManager
		{
		public:

			/*!
			Get a shared pointer to the global instnce, use GetVE() instead.
			*/
			static std::shared_ptr<VeManager> GetInstance();
			~VeManager();

			/*!
			Get a shared pointer to the global VirtualEnvironmentAPI.
			*/
			std::shared_ptr<VirtualEnvironmentAPI> GetVE();

		private:
			VeManager();
			static std::shared_ptr<VeManager> Instance;
			std::shared_ptr<VirtualEnvironmentAPI> VirtualEnvironment;
		};


		/*!
		Get a shared pointer to the global VirtualEnvironmentAPI.
		*/
		inline std::shared_ptr<VirtualEnvironmentAPI> VE()
		{
			return VeManager::GetInstance()->GetVE();
		}
	}
}

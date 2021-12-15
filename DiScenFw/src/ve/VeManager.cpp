//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#include <discenfw/ve/VeManager.h>

#include <discenfw/ve/VirtualEnvironmentAPI.h>

namespace discenfw
{
	namespace ve
	{
		std::shared_ptr<VeManager> VeManager::Instance;

		std::shared_ptr<VeManager> VeManager::GetInstance()
		{
			if (!Instance)
			{
				Instance = std::shared_ptr<VeManager>(new VeManager());
			}
			return Instance;
		}


		VeManager::VeManager()
		{
		}

		VeManager::~VeManager()
		{
		}

		std::shared_ptr<VirtualEnvironmentAPI> VeManager::GetVE()
		{
			if (!VirtualEnvironment)
			{
				VirtualEnvironment = std::make_shared<VirtualEnvironmentAPI>();
			}

			return VirtualEnvironment;
		}
	}
}


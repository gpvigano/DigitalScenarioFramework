//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/scen/EntityBadge.h"
#include "discenfw/scen/ScenarioData.h"

#include <string>

namespace discenfw
{
	/*!
	Inventory item information, used to pick items from inventory.
	*/
	struct InventoryItem
	{
		//! Information used to display the item.
		EntityBadge Badge;

		//! Entity reference.
		std::shared_ptr<Entity> Item;

		//! True if the item was added to the scenario, false if it is contained in the inventory.
		bool InScenario = false;
	};
}

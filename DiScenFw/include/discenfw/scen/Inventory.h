//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/scen/InventoryItem.h"

#include <string>
#include <vector>

namespace discenfw
{
	/*!
	Inventory containing items that can be added to the scenario.
	*/
	struct Inventory
	{
		//! Items information.
		std::vector< std::shared_ptr<InventoryItem> > Items;

		/*!
		Grouping flag.
		It is true if items with the same type and configuration are grouped together (e.g. showing an icon with the number of items),
		false otherwise (e.g. they have more icons for items of the same type).
		*/
		bool GroupByType = false;
	};
}

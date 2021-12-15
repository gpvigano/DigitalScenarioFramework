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
#include "discenfw/scen/AssetDefinition.h"

#include <string>

namespace discenfw
{
	/*!
	Catalog item data.
	*/
	struct CatalogItem
	{
		//! Information used to display the item.
		EntityBadge Badge;

		//! Reference to the asset where the target entity is defined.
		AssetDefinition Asset;

		//! This item must be added first, when the scenario is empty.
		bool FirstOne = false;

		//! Number of allowed instances (negative = infinite, default).
		int AllowedInstances = -1;

		//! Hidden in the user interface (useful to hide the item while keeping its definition).
		bool Hidden = false;
	};
}

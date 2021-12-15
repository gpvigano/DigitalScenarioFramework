//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/scen/EntityIdentity.h"
#include <string>

namespace discenfw
{
	/*!
	Entity "badge", used to summarize entity information.
	*/
	struct EntityBadge : public EntityIdentity
	{
		/*!
		Name used to display the entity.
		*/
		std::string DisplayName;

		/*!
		Name of the displayed icon.
		*/
		std::string Icon;


		/// @name Constructors.
		///@{

		EntityBadge() {}

		EntityBadge(const std::string& id, const std::string entType)
			: EntityIdentity(id,entType)
		{
		}

		EntityBadge(
			const std::string& id,
			const std::string& entType,
			const std::string& category,
			const std::string& description,
			const std::string& displayName,
			const std::string& icon
			)
			: EntityIdentity(id, entType, category, description),
			DisplayName(displayName),
			Icon(icon)
		{
		}

		///@}
	};
}

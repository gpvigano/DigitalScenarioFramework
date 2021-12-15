//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include <string>

namespace discenfw
{
	/*!
	Information about entity identity.
	*/
	struct EntityIdentity
	{
		/*!
		Unique identifier of the entity.
		*/
		std::string Identifier;

		/*!
		Entity type.
		*/
		std::string Type;

		/*!
		Category used to group entities (it refers to one or more types).
		*/
		std::string Category;

		/*!
		Brief description.
		*/
		std::string Description;


		/// @name Constructors.
		///@{

		EntityIdentity() {}

		EntityIdentity(const std::string& id, const std::string entType)
			: Identifier(id), Type(entType)
		{
		}

		EntityIdentity(
			const std::string& id,
			const std::string& entType,
			const std::string& category,
			const std::string& description
			)
			: Identifier(id),
			Type(entType),
			Category(category),
			Description(description)
		{
		}

		///@}
	};
}

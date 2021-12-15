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
	namespace xp
	{
		/*!
		Link to an entity and its link in a relationship.
		*/
		struct RelationshipLink
		{
			/*!
			Identifier of the related entity.
			*/
			std::string EntityId;

			/*!
			Identifier of the link of the related entity connected to the local link.
			*/
			std::string LinkId;

			RelationshipLink()
			{
			}

			RelationshipLink(const std::string& entityId, const std::string& linkId)
				: EntityId(entityId), LinkId(linkId)
			{
			}

			bool operator ==(const RelationshipLink& link) const;
		};

		inline bool RelationshipLink::operator ==(const RelationshipLink& link) const
		{
			return EntityId == link.EntityId && LinkId == link.LinkId;
		}
	}
}


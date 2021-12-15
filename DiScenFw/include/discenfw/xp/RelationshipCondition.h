//--------------------------------------------------------------------//
// Digital Scenario Framework                                         //
//  by Giovanni Paolo Vigano', 2021                                   //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

#pragma once

#include "discenfw/xp/RelationshipLink.h"

#include <string>

namespace discenfw
{
	namespace xp
	{
		/*!
		Condition checking if there is a specified relationship between two entities.
		*/
		struct RelationshipCondition
		{
			/*!
			Link to the relating entity.
			*/
			std::string RelatingLinkId;

			/*!
			Link to the related entity.
			*/
			RelationshipLink Related;


			RelationshipCondition()
			{
			}

			RelationshipCondition(
				const std::string& linkId,
				const RelationshipLink& other,
				bool unrelated = false)
				: RelatingLinkId(linkId), Related(other)
			{
			}

			RelationshipCondition(
				const std::string& linkId,
				const std::string& otherEntityId,
				const std::string& otherLinkId,
				bool unrelated = false)
				: RelatingLinkId(linkId), Related(otherEntityId,otherLinkId)
			{
			}

			/*!
			If true the condition is inverted: it is  true if the specified relationship is not defined.
			*/
			bool Unrelated = false;

			bool operator ==(const RelationshipCondition& relCondition) const;

			bool Evaluate(const std::string& relating, const RelationshipLink& related) const;
		};


		inline bool RelationshipCondition::operator ==(const RelationshipCondition& relCondition) const
		{
			return Unrelated == relCondition.Unrelated && RelatingLinkId == relCondition.RelatingLinkId && Related == relCondition.Related;
		}


		inline bool RelationshipCondition::Evaluate(const std::string& relatingLink, const RelationshipLink& relatedLink) const
		{
			bool related;
			if(RelatingLinkId.empty())
			{
				related = (Related.EntityId == relatedLink.EntityId);
			}
			else
			{
				related = (RelatingLinkId == relatingLink && Related == relatedLink);
			}
			return Unrelated ? !related : related;
		}
	}
}
